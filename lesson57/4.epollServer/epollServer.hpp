#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/epoll.h>
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

    public:
        EpollServer(uint16_t port = defaultPort)
            : _port(port)
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
            ev.data.fd = _listenSock;
            epoll_ctl(_epfd, EPOLL_CTL_ADD, _listenSock, &ev);
        }

        void start()
        {
            for(;;)
            {
                int n = epoll_wait(_epfd,)
            }
        }

        ~EpollServer()
        {
            if(_listenSock != defaultValue) close(_listenSock);
            if(_epfd != defaultValue) close(_epfd);
        }

    private:
        uint16_t _port;
        int _listenSock;
        int _epfd;
    };
}