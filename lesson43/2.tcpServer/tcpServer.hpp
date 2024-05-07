#pragma once

#include "log.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace server
{
    using namespace std;

    enum {
        USAGE_ERR = 1,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR
    };

    static const uint16_t gport = 8080;
    static const int gbacklog = 5;

    class TcpServer
    {
    public:

        TcpServer(const uint16_t& port = gport)
            :_port(port)
            ,_listenSockfd(-1)
        {}

        void initServer()
        {
            // 1.创建socket文件套接字对象
            _listenSockfd = socket(AF_INET, SOCK_STREAM, 0); // 第二个参数与UDP不同
            if(_listenSockfd < 0)
            {
                // 创建套接字失败
                logMessage(FATAL, "created socket error!");
                exit(SOCKET_ERR);
            }
            logMessage(NORMAL, "created socket success!");

            // 2.bind绑定自己的网络信息
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if(bind(_listenSockfd, (struct sockaddr*)&local, sizeof(local)) < 0)
            {
                logMessage(FATAL, "bind socket error!");
                exit(BIND_ERR);
            }
            logMessage(NORMAL, "bind socket success!");

            // 3.设置socket 为监听状态
            if(listen(_listenSockfd, gbacklog) < 0) // 第二个参数backlog后面会讲 5的倍数
            {
                logMessage(FATAL, "listen socket error!");
                exit(LISTEN_ERR);
            }
            logMessage(NORMAL, "listen socket success!");
        }

        void start()
        {
            for( ; ; )
            {
                // 4.server 获取新连接 不能直接接收数据/发送数据
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                int sock = accept(_listenSockfd, (struct sockaddr*)&peer, &len); //sock 和client进行通信
                if(sock < 0)
                {
                    logMessage(ERROR, "accept error, next!");
                    continue;
                }
                logMessage(NORMAL, "accept a new link success!"); // ?
                cout << "sock: " << sock << endl;

                // 5.这里就是一个sock， 未来通信通sock，面向字节流的，后续全部都是文件操作！
                // verson1
                serviceIO(sock);
                close(sock); // 对一个已经使用完毕的sock 要关闭这个sock 要不然会导致文件描述符泄露
            }
        }
        
        void serviceIO(int sock)
        {
            char buffer[1024];
            while(true)
            {
                ssize_t n = read(sock, buffer, sizeof(buffer)-1);
                if(n > 0)
                {
                    // 目前把独到的数据当成字符串 截至目前
                    buffer[n] = 0;
                    cout << "recv message:" << buffer << endl;

                    string outBuffer = buffer;
                    outBuffer += "  server[echo]";

                    write(sock, outBuffer.c_str(), outBuffer.size());
                }
                else if(n == 0)
                {
                    // 代表client退出
                    logMessage(NORMAL, "client quit, me too!");
                    break;
                }
            }           

        }

        ~TcpServer()
        {}
    private:
        int _listenSockfd; // 套接字 -- 不是用来通信的 是用来监听链接到来，获取新链接的！
        uint16_t _port; // 端口号
    };
}