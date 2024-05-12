#pragma once

#include "log.hpp"
#include "protocol.hpp"

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

    typedef std::function<bool(const Request &req, Response &resp)> func_t;

    // 解耦
    void handlerEntery(int sock, func_t func)
    {
        std::string inbuffer;
        while (true)
        {
            // 1.读取，content_len"\r\n"_exitcode _result"\r\n
            // 1.1 如何保证独到的消息是 一个！完整的请求呢？
            std::string req_text, req_str;
            // 1.2 读取成功，req_text是一个完整的请求：content_len"\r\n"_exitcode _result"\r\n
            if (!recvPackge(sock, inbuffer, &req_text))
                return; // 读取失败
            std::cout << "带报头的请求： \n" << req_text << std::endl;
            if (!deLength(req_text, &req_str))
                return;
            std::cout << "去报头的正文： \n" << req_str << std::endl;

            // 2.对请求request反序列化
            // 2.1 得到一个结构化的请求对象
            Request req;
            if (!req.deserialize(req_str))
                return;

            // 3.计算处理业务 req._x, req._op, req._y --- 业务逻辑
            // 3.1 得到一个结构化的响应
            Response resp;
            func(req, resp); // 调用的为.cc中的cal

            // 4.对相应Response进行序列化
            // 4.1 得到了一个序列化的数据
            std::string resp_str;
            resp.serialize(&resp_str);

            std::cout << "计算完成，序列化响应：" << req_str << std::endl;

            // 5.然后发送响应给客户端
            // 5.1构建成为一个完整的报文
            std::string send_string = enLength(resp_str);
            std::cout << "构建完整的响应：\n" << send_string << std::endl;

            send(sock, send_string.c_str(), send_string.size(), 0); // 这里的发送也是有问题的
        }
    }

    class CalServer
    {
    public:
        CalServer(const uint16_t &port = gport)
            : _port(port), _listenSockfd(-1)
        {
        }

        void initServer()
        {
            // 1.创建socket文件套接字对象
            _listenSockfd = socket(AF_INET, SOCK_STREAM, 0); // 第二个参数与UDP不同
            if (_listenSockfd < 0)
            {
                // 创建套接字失败
                logMessage(FATAL, "created socket error!");
                exit(SOCKET_ERR);
            }
            logMessage(NORMAL, "created socket success: %d!", _listenSockfd);

            // 2.bind绑定自己的网络信息
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if (bind(_listenSockfd, (struct sockaddr *)&local, sizeof(local)) < 0)
            {
                logMessage(FATAL, "bind socket error!");
                exit(BIND_ERR);
            }
            logMessage(NORMAL, "bind socket success!");

            // 3.设置socket 为监听状态
            if (listen(_listenSockfd, gbacklog) < 0) // 第二个参数backlog后面会讲 5的倍数
            {
                logMessage(FATAL, "listen socket error!");
                exit(LISTEN_ERR);
            }
            logMessage(NORMAL, "listen socket success!");
        }

        void start(func_t func)
        {
            for (;;)
            {
                // 4.server 获取新连接 不能直接接收数据/发送数据
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                int sock = accept(_listenSockfd, (struct sockaddr *)&peer, &len); // sock 和client进行通信
                if (sock < 0)
                {
                    logMessage(ERROR, "accept error, next!");
                    continue;
                }
                logMessage(NORMAL, "accept a new link success, get new sock: %d!", sock); // ?

                // version 2, 多进程版，
                pid_t id = fork();
                if (id == 0)
                {
                    // 子进程 向外提供服务 不需要监听 关闭这个文件描述符
                    close(_listenSockfd);
                    // if(fork() > 0) exit(0); // 让子进程的子进程执行下面代码 子进程退出
                    // serviceIO(sock);
                    handlerEntery(sock, func); // 读取请求
                    close(sock);               // 关闭父进程的
                    exit(0);                   // 最后变成孤儿进程 交给OS回收这个进程
                }
                close(sock); // 关闭子进程的

                // 父进程
                pid_t ret = waitpid(id, nullptr, 0); // 阻塞式等待
                if (ret > 0)
                {
                    logMessage(NORMAL, "wait child process seccess");
                }
            }
        }

        ~CalServer()
        {
        }

    private:
        int _listenSockfd; // 套接字 -- 不是用来通信的 是用来监听链接到来，获取新链接的！
        uint16_t _port;    // 端口号
    };
}