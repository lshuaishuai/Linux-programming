#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <poll.h>
#include <functional>

#include "log.hpp"
#include "sock.hpp"

namespace poll_ns
{
    class PollServer
    {
        const static uint16_t defaultport = 8888;
        const static int num = 2048; // _rfds的大小
        const static int defaultfd = -1;

        using func_t = std::function<std::string (const std::string&)>;

    public:
        PollServer(func_t func, uint16_t port = defaultport)
            :_port(port)
            ,_listensock(-1)
            ,_rfds(nullptr)
            ,_func(func)
        {}

        void ReItem(int i)
        {
            _rfds[i].fd = defaultfd;
            _rfds[i].events = 0;
            _rfds[i].revents = 0;
        }

        void initServer()
        {
            _listensock = Sock::Socket();
            Sock::Bind(_listensock, _port);
            Sock::Listen(_listensock);

            _rfds = new struct pollfd[num];
            for(int i = 0; i < num; i++) ReItem(i);

            _rfds[0].fd = _listensock;  // 监听事件就绪
            _rfds[0].events = POLLIN;
        }

        void HandlerReadEvents()
        {
            for(int i = 0; i < num; i++)
            {
                if(_rfds[i].fd == defaultfd) continue;
                if(!(_rfds[i].revents & POLLIN)) continue; // _rfds[i].fd关心的事件没有就绪的话，则跳过

                if(_rfds[i].fd == _listensock && (_rfds[i].revents & POLLIN)) 
                    Accepter();
                else if(_rfds[i].revents & POLLIN)
                    Recver(_rfds[i].fd, i);
                else
                {

                }
            }
        }

        void Print()
        {
            std::cout << "fd list: ";
            for(int i = 0; i < num; i++)
            {
                if(_rfds[i].fd == defaultfd) continue;
                std::cout<< _rfds[i].fd <<" ";
            }
            std::cout << std::endl;
        }

        void Accepter()
        {
            std::string clientip;
            uint16_t clientport;

            int sock = Sock::Accept(_listensock, &clientip, &clientport);
            if(sock < 0) return;
            logMessage(NORMAL, "accept success [%s, %d]", clientip.c_str(), clientport);

            // 将新的sock设置到数组中
            int i = 0; 
            for(; i < num; i++)
            {
                if(_rfds[i].fd != defaultfd) continue;
                else break;
            }

            if(i == num)
            {
                logMessage(WARNING, "server is full, please wait!");
                close(sock);
            }
            else{
                _rfds[i].fd = sock;
                _rfds[i].events = POLLIN;
                _rfds[i].revents = 0;
            }
            Print();
        }

        void Recver(int sock, int pos)
        {
            // 1.读取数据
            char buffer[1024];
            int s = recv(sock, buffer, sizeof(buffer)-1, 0);
            if(s > 0)
            {
                buffer[s] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if(s == 0) // 返回值为0表示链接已经关闭了
            {
                close(sock);
                logMessage(NORMAL, "client quit!");
                ReItem(pos);
                return;
            }
            else{
                close(sock);
                logMessage(ERROR, "client quit: %s", strerror(errno));
                return;
            }

            // 2.处理数据
            std::string respone = _func(buffer);

            // 3.给client响应
            write(sock, respone.c_str(), respone.size());

        }

        void start()
        {
            int timeout = -1;
            for(;;)
            {
                int n = poll(_rfds, num, timeout);
                switch (n)
                {
                case 0:
                    logMessage(NORMAL, "timeout...");
                    break;
                case -1:
                    logMessage(WARNING, "poll called fail! errno: %d, strerror: %s", errno, strerror(errno));
                    break;
                default:
                    logMessage(NORMAL, "poll called success! get a new link...");
                    HandlerReadEvents();
                    break;
                }
            }
        }

        ~PollServer()
        {}

    private:
        uint16_t _port;
        int _listensock;
        struct pollfd *_rfds;
        func_t _func;
    };
}