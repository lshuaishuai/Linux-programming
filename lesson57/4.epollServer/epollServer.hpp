#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/epoll.h>
#include <functional>

#include "err.h"
#include "log.hpp"
#include "sock.hpp"

namespace epoll_ns
{
    class EpollServer
    {
        static const int defaultPort = 8081;
        static const int size = 128;
        static const int defaultValue = -1;
        static const int defaultnum = 64;

        using func_t = std::function<std::string (const string&)>;

    public:
        EpollServer(func_t func, uint16_t port = defaultPort, int num = defaultnum)
            :_func(func)
            ,_num(num)
            ,_port(port)
        {
        }

        void initServer()
        {
            // 1.创建socket
            _listenSock = Sock::Socket();
            Sock::Bind(_listenSock, _port);
            Sock::Listen(_listenSock);

            // 2.创建epoll模型
            _epfd = epoll_create(size);
            if(_epfd < 0)
            {
                logMessage(FATAL, "epoll create error: %s", strerror(errno));
                exit(EPOLL_CREATE_ERR);
            }

            // 3.添加listensock到epoll中
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = _listenSock; // 当事件就绪 被重新捞取上来的时候，我们要知道是哪一个fd就绪了
            epoll_ctl(_epfd, EPOLL_CTL_ADD, _listenSock, &ev);

            // 4.申请就绪时间的空间
            _revs = new struct epoll_event[_num];

            logMessage(NORMAL, "init server success!");
        }

        void Print(int readyNum)
        {
            std::cout << "fd list: ";
            for(int i = 0; i < readyNum; i++)
            {
                std::cout << _revs[i].data.fd << " ";
            }
            std::cout << std::endl;
        }

        void HandlerEvent(int readyNum)
        {
            logMessage(DEBUG, "Handler in");
            for(int i = 0; i < readyNum; i++)
            {
                uint32_t events = _revs[i].events; 
                int sock = _revs[i].data.fd;
                Print(readyNum);
                if(sock == _listenSock && (events & EPOLLIN))
                {
                    // _listensock的读事件就绪，获取新连接
                    std::string clientip;
                    uint16_t clientport;
                    int fd = Sock::Accept(sock, &clientip, &clientport);

                    if(fd < 0)
                    {
                        logMessage(WARNING, "accept error");
                        continue;
                    }
                    // 获取新的fd成功，可以直接读取吗？不能直接读取，可能会阻塞，将其放入epoll中
                    struct epoll_event ev;
                    ev.events = EPOLLIN;
                    ev.data.fd = fd;
                    epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
                }
                else if(events & EPOLLIN)
                {
                    // 普通的读事件就绪
                    // 读取是有问题的 没有协议定制
                    logMessage(DEBUG, "recv start");
                    char buffer[1024];
                    // 把本轮读完也不一定读到一个完整的请求
                    int n = recv(sock, buffer, sizeof(buffer)-1, 0);
                    if(n > 0)
                    {
                        buffer[n] = 0;
                        logMessage(DEBUG, "client# %s", buffer);

                        std::string response = _func(buffer);

                        send(sock, response.c_str(), response.size(), 0);
                    }
                    else if(n == 0)
                    {
                        // 建议先从epoll溢出，才close 文件描述符
                        epoll_ctl(_epfd, EPOLL_CTL_DEL, sock, nullptr);
                        close(sock);
                        logMessage(NORMAL, "client quit");
                    }
                    else
                    {
                        epoll_ctl(_epfd, EPOLL_CTL_DEL, sock, nullptr);
                        close(sock);
                        logMessage(ERROR, "recv error, code: %d, errstring: %s", errno, strerror(errno));
                    }
                }
                else if(events & EPOLLOUT)
                {}
            }
            logMessage(DEBUG, "Handler out");
        }

        void start()
        {
            int timeout = -1;
            for(;;)
            {
                int n = epoll_wait(_epfd, _revs, _num, timeout);
                switch(n)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "epoll_wait failed, code: %d, errstring: %s", errno, strerror(errno));
                    break;
                default:
                    logMessage(NORMAL, "have event ready");
                    HandlerEvent(n); // 有n个事件就绪
                    break;
                }
            }
        }

        ~EpollServer()
        {
            if(_listenSock != defaultValue) close(_listenSock);
            if(_epfd != defaultValue) close(_epfd);
            if(_revs) delete[] _revs;
        }

    private:
        uint16_t _port;
        int _listenSock;
        int _epfd;
        struct epoll_event *_revs;
        int _num;
        func_t _func;
    };
}