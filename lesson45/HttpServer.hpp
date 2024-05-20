#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <functional>
#include <cstdlib>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "Protocol.hpp"

namespace server
{

    using namespace std;

    enum
    {
        USAGE_ERR = 1,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR
    };

    static const uint16_t gport = 8080;
    static const int gbacklog = 5;

    using func_t = std::function<bool (const HttpRequest &req, HttpResponse &resp)>;

    class HttpServer
    {
    public:
        HttpServer(){}
        
        HttpServer(func_t func, const uint16_t &port = gport)
            : _func(func)
            ,_port(port)
            , _listenSockfd(-1)
        {
        }

        void initServer()
        {
            // 1.创建socket文件套接字对象
            _listenSockfd = socket(AF_INET, SOCK_STREAM, 0); // 第二个参数与UDP不同
            if (_listenSockfd < 0)
            {
                // 创建套接字失败
                exit(SOCKET_ERR);
            }

            // 2.bind绑定自己的网络信息
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if (bind(_listenSockfd, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                exit(BIND_ERR);
            }

            // 3.设置socket 为监听状态
            if (listen(_listenSockfd, gbacklog) < 0) // 第二个参数backlog后面会讲 5的倍数
            {
                exit(LISTEN_ERR);
            }
        }

        void HandlerHttp(int sock)
        {
            // 1.读到完整的http请求
            // 2.反序列化 inbuffer
            // 3.httpRequest， httpresponse， _func(req, resp)
            // 4.将响应序列化 outbuffer
            // 5.send 发送

            char buffer[4096];
            HttpRequest req;
            HttpResponse resp;
            size_t n = recv(sock, buffer, sizeof(buffer) - 1, 0); 
            if(n > 0)
            {
                buffer[n] = 0;
                req.inbuffer = buffer;
                req.parse();
                _func(req, resp);  // 根据req得到resp
                send(sock, resp.outbuffer.c_str(), resp.outbuffer.size(), 0);
            }
        }

        void start()
        {
            for (;;)
            {
                // 4.server 获取新连接 不能直接接收数据/发送数据
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                int sock = accept(_listenSockfd, (struct sockaddr *)&peer, &len); // sock 和client进行通信
                if (sock < 0)
                {
                    continue;
                }

                // version 2, 多进程版，
                pid_t id = fork();
                if (id == 0)
                {
                    // 子进程 向外提供服务 不需要监听 关闭这个文件描述符
                    close(_listenSockfd);
                    if(fork() > 0) exit(0); // 让子进程的子进程执行下面代码 子进程退出
                    HandlerHttp(sock);
                    close(sock);               // 关闭父进程的
                    exit(0);                   // 最后变成孤儿进程 交给OS回收这个进程
                }
                close(sock); // 关闭子进程的

                // 父进程
                waitpid(id, nullptr, 0); // 阻塞式等待
            }
        }

        ~HttpServer()
        {
        }

    private:
        int _listenSockfd; // 套接字 -- 不是用来通信的 是用来监听链接到来，获取新链接的！
        uint16_t _port;    // 端口号
        func_t _func;
    };
}