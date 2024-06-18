#pragma once

#include <iostream>
#include <sys/epoll.h>
#include <string>
#include <cstring>

#include "Err.hpp"
#include "Log.hpp"

const static int defaultepfd = -1;
const static int size = 128;

class Epoller
{
public:
    Epoller()
        :_epfd(defaultepfd)
    {}

    ~Epoller()
    {
        if(_epfd != defaultepfd) close(_epfd);
    }

public:
    void Create()
    {
        _epfd = epoll_create(size);
        if(_epfd < 0)
        {
            logMessage(FATAL, "epoll created error, code: %d, errstring: %s", errno, strerror(errno));
            exit(EPOLL_CREATE_ERR);
        }
    }

    // user -> kernel
    bool AddEvent(int sock, uint32_t events)
    {
        struct epoll_event ev;
        ev.data.fd = sock;
        ev.events = events;

        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, sock, &ev);
        return n == 0; // 等于0添加成功
    }

private:
    int _epfd;
};