#pragma once

#include <string>
#include <iostream>
#include <functional>
#include "sock.hpp"
#include "log.hpp"
#include "err.hpp"

namespace poll_ns
{
    static const int defaultport = 8080;
    static const int num = 2048;
    static const int defaultfd = -1;

    using func_t = std::function<std::string (const std::string&)>;

    class PollServer
    {
    public:
        PollServer(func_t func, int port = defaultport)
            : _port(port), _listenSock(-1), _func(func), _rfds(nullptr)
        {
        }

        void initServer()
        {
            _listenSock = Sock::Socket();
            Sock::Bind(_listenSock, _port);
            Sock::Listen(_listenSock);
            // logMessage(NORMAL, "creat socket..");
            _rfds = new struct pollfd[num];
            for (int i = 0; i < num; i++) ResetItem(i);

            _rfds[0].fd = _listenSock; // 不变了
            _rfds[0].events = POLLIN;
        }

        void Print()
        {
            std::cout << "fd list: ";
            for (int i = 0; i < num; i++)
            {
                if (_rfds[i].fd != defaultfd)
                    std::cout << _rfds[i].fd << " ";
            }
            std::cout << std::endl;
        }

        void ResetItem(int i)
        {
            _rfds[i].fd = defaultfd;
            _rfds[i].events = 0;
            _rfds[i].revents = 0;
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
            for (i = 0; i < num; i++)
            {
                if (_rfds[i].fd != defaultfd)
                    continue;
                else
                    break;
            }
            if (i == num)
            {
                logMessage(WARNING, "server is full, please wait!");
                close(sock);
            }
            else
            {
                _rfds[i] .fd= sock;
                _rfds[i].events = POLLIN;
                _rfds[i].revents = 0;
            }
            Print();
        }

        void Recver(int pos)
        {
            // 1.读取
            // 这样读取有问题！不能保证是否读取到一个完整的报文
            char buffer[1024];
            ssize_t s = recv(_rfds[pos].fd, buffer, sizeof(buffer) - 1, 0); // 这里在进行recv'时，不会被阻塞，因为走到这里时文件描述符已经就绪了
            if (s > 0)
            {
                buffer[s] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if (s == 0)
            {
                close(_rfds[pos].fd);
                ResetItem(pos);
                logMessage(NORMAL, "client quit");
                return;
            }
            else
            {
                close(_rfds[pos].fd);
                ResetItem(pos);
                logMessage(ERROR, "client quit: %s", strerror(errno));
                return;
            }

            // 2.处理request
            std::string response = _func(buffer);
            
            // 3.返回response
            // write
            write(_rfds[pos].fd, response.c_str(), response.size());
        }

        // handler event 中 不仅仅是有一个fd就绪，可能有多个
        // 我们的select只处理了read
        void HandlerReadEvent()
        {
            for (int i = 0; i < num; i++)
            {
                // 过滤掉非法的fd
                if (_rfds[i].fd == defaultfd)
                    continue;
                if(!(_rfds[i].revents & POLLIN)) continue;
                // 下面的为正常的fd
                // 正常的fd不一定就绪
                // 目前一定是listen套接字
                if (_rfds[i].fd == _listenSock && (_rfds[i].revents & POLLIN))
                    Accepter(_listenSock);
                else if(_rfds[i].revents & POLLIN)
                    Recver(i);
                else
                {
                        
                }
            }
        }

        void start()
        {
            int timeout = -1;
            for (;;)
            {
                int n = poll(_rfds, num, timeout);
                switch (n)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "poll error, code: %d, err string: %s", errno, strerror(errno));
                    break;
                default:
                    // 说明有时间就绪了，目前只有一个监听事件就绪
                    logMessage(NORMAL, "get a new link...");
                    HandlerReadEvent();
                    break;
                }
            }
        }

        ~PollServer()
        {
            if (_listenSock < 0)
                close(_listenSock);
            if (_rfds)
                delete[] _rfds;
        }

    private:
        int _port;
        int _listenSock;
        struct pollfd* _rfds;
        func_t _func;
    };
}