#pragma once

#include <string>
#include <iostream>
#include <functional>
#include "sock.hpp"
#include "log.hpp"
#include "err.hpp"

namespace select_ns
{
    static const int defaultport = 8080;
    static const int fd_num = sizeof(fd_set) * 8;
    static const int defaultfd = -1;

    using func_t = std::function<std::string (const std::string&)>;

    class SelectServer
    {
    public:
        SelectServer(func_t func, int port = defaultport)
            : _port(port), _listenSock(-1), _fdArray(nullptr), _func(func)
        {
        }

        void Print()
        {
            std::cout << "fd list: ";
            for (int i = 0; i < fd_num; i++)
            {
                if (_fdArray[i] != defaultfd)
                    std::cout << _fdArray[i] << " ";
            }
            std::cout << std::endl;
        }

        void Accepter(int listenSock)
        {
            // 走到这里accept不会阻塞 listensock套接字已经就绪了
            string clientIp;
            uint16_t clientPort = 0;
            int sock = Sock::Accept(listenSock, &clientIp, &clientPort);
            if (sock < 0)
                return;
            logMessage(NORMAL, "accept success [%s:%d]", clientIp.c_str(), clientPort);

            // sock 我们能直接recv/read吗？--不能 整个代码 只有select有资格检测事件是否就绪
            // 将新的sock交给select
            // 将新的sock托管给select的本质，将sock添加到_fdArray数组中
            int i;
            for (i = 0; i < fd_num; i++)
            {
                if (_fdArray[i] != defaultfd)
                    continue;
                else
                    break;
            }
            if (i == fd_num)
            {
                logMessage(WARNING, "server is full, please wait!");
                close(sock);
            }
            else
            {
                _fdArray[i] = sock;
            }
            Print();
        }

        void Recver(int sock, int pos)
        {
            // 1.读取
            // 这样读取有问题！不能保证是否读取到一个完整的报文
            char buffer[1024];
            ssize_t s = recv(sock, buffer, sizeof(buffer) - 1, 0); // 这里在进行recv'时，不会被阻塞，因为走到这里时文件描述符已经就绪了
            if (s > 0)
            {
                buffer[s] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if (s == 0)
            {
                close(sock);
                _fdArray[pos] = defaultfd;
                logMessage(NORMAL, "client quit");
                return;
            }
            else
            {
                close(sock);
                _fdArray[pos] = defaultfd;
                logMessage(ERROR, "client quit: %s", strerror(errno));
                return;
            }

            // 2.处理request
            std::string response = _func(buffer);
            
            // 3.返回response
            // write
            write(sock, response.c_str(), response.size());
        }

        // handler event 中 不仅仅是有一个fd就绪，可能有多个
        // 我们的select只处理了read
        void HandlerReadEvent(fd_set &rfds)
        {
            for (int i = 0; i < fd_num; i++)
            {
                // 过滤掉非法的fd
                if (_fdArray[i] == defaultfd)
                    continue;

                // 下面的为正常的fd
                // 正常的fd不一定就绪
                // 目前一定是listen套接字
                if (FD_ISSET(_fdArray[i], &rfds) && _fdArray[i] == _listenSock)
                    Accepter(_listenSock);
                else
                    Recver(_fdArray[i], i);
            }
        }

        void initServer()
        {
            _listenSock = Sock::Socket();
            Sock::Bind(_listenSock, _port);
            Sock::Listen(_listenSock);
            // logMessage(NORMAL, "creat socket..");
            _fdArray = new int[fd_num];
            for (int i = 0; i < fd_num; i++)
                _fdArray[i] = defaultfd;
            _fdArray[0] = _listenSock; // 不变了
        }

        void start()
        {
            for (;;)
            {
                fd_set rfds;
                FD_ZERO(&rfds);
                int maxfd = _fdArray[0];
                for (int i = 0; i < fd_num; i++)
                {
                    if (defaultfd == _fdArray[fd_num])
                        continue;
                    FD_SET(_fdArray[i], &rfds); // 将合法fd全部添加到读文件描述符集中
                    if (maxfd < _fdArray[i])
                        maxfd = _fdArray[i];
                }

                // struct timeval timeout = {1, 0};
                // int n = select(_listenSock+1, &rfds, nullptr, nullptr, &timeout);
                // 一般而言 要使用select 需要程序员维护一个保存所有合法fd 的数组！
                int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);
                switch (n)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "select error, code: %d, err string: %s", errno, strerror(errno));
                    break;
                default:
                    // 说明有时间就绪了，目前只有一个监听事件就绪
                    logMessage(NORMAL, "get a new link...");
                    HandlerReadEvent(rfds);
                    break;
                }
                sleep(1);

                // std::string clientIp;
                // uint16_t clientPort = 0;
                // int sock = Sock::Accept(_listenSock, &clientIp, &clientPort);
                // if(sock < 0) continue;
                // // 开始进行服务器处理逻辑
            }
        }

        ~SelectServer()
        {
            if (_listenSock < 0)
                close(_listenSock);
            if (_fdArray)
                delete[] _fdArray;
        }

    private:
        int _port;
        int _listenSock;
        int *_fdArray;
        func_t _func;
    };
}