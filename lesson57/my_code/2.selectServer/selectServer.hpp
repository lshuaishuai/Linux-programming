#pragma once

#include <iostream>
#include <functional>
#include <sys/select.h>

#include "sock.hpp"

namespace select_ns
{
    class SelectServer
    {
        static const int defaultPort = 8081;
        static const int defaultfd = -1;
        static const int fdNum = sizeof(fd_set) * 8;

        using func_t = std::function<std::string (const std::string&)>;

    public:
        SelectServer(func_t func, int port = defaultPort)
            :_port(port)
            ,_listenSock(-1)
            ,_fdArray(nullptr)
            ,_func(func)
        {}

        void initServer()
        {
            _listenSock = Sock::Socket();
            Sock::Bind(_listenSock, _port);
            Sock::Listen(_listenSock);

            _fdArray = new int[fdNum];
            for(int i = 0; i < fdNum; i++)
                _fdArray[i] = defaultfd;
            _fdArray[0] = _listenSock; // 将listensock设置为第一个
            // logMessage(DEBUG, "initServer success");
        }

        void Print()
        {
            std::cout << "fd list: ";
            for(int i = 0; i < fdNum; i++)
            {
                if(_fdArray[i] == defaultfd) continue;
                std::cout << _fdArray[i] << " ";
            }
            std::cout << std::endl;
        }

        void Accepter(int listensock)
        {
            string clientip;
            uint16_t clientport = 0;
            int sock = Sock::Accept(listensock, &clientip, &clientport);

            if(sock < 0) return;
            logMessage(NORMAL, "accept success [%s, %d] ", clientip.c_str(), clientport);

            // 然后再将新的sock交给select，就是设置_fdArray
            int i = 0; 
            for(; i < fdNum; i++)
            {
                if(_fdArray[i] != defaultfd) continue; // 找到第一个未被设置的fd位置
                else break;
            }
            
            if(i == fdNum) // 此时fd数组已经满了不能再设置新的fd
            {
                logMessage(WARNING, "server is full, please wait");
                close(sock);
            }
            else _fdArray[i] = sock;

            Print();
        }

        void Recver(int sock, int pos)
        {
            // 1.读数据
            char buffer[1024];
            ssize_t s = recv(sock, buffer, sizeof(buffer)-1, 0); // 阻塞式读取
            if(s > 0)
            {
                buffer[s] = 0;
                logMessage(NORMAL, "client# %s", buffer);
            }
            else if(s == 0)
            {
                close(sock);
                _fdArray[pos] = defaultfd;
                logMessage(NORMAL, "client quit!");
                return;
            }
            else{
                close(sock);
                _fdArray[pos] = defaultfd;
                logMessage(ERROR, "client quit: %s", strerror(errno));
                return;
            }

            // 2.处理数据
            string respone = _func(buffer);

            // 3.返回应答respone
            write(sock, respone.c_str(), respone.size());
        }

        void HandlerReadEvents(fd_set rfds)
        {
            for(int i = 0; i < fdNum; i++)
            {
                if(_fdArray[i] == defaultfd) continue;
                if(FD_ISSET(_fdArray[i], &rfds) && _fdArray[i] == _listenSock) // 只有监听sock才需要accept
                    Accepter(_listenSock);
                else
                    Recver(_fdArray[i], i);
            }
        }

        void start()
        {
            logMessage(NORMAL, "Server start...");
            // server启动 一定是一直运行的 for循环
            for(;;)
            {
                fd_set rfds;
                FD_ZERO(&rfds);
                // 首先找出数组中的最大的fd 设置select的首个参数
                int maxfd = _fdArray[0];
                for(int i = 0; i < fdNum; i++)
                {
                    if(_fdArray[i] == defaultfd) continue;
                    FD_SET(_fdArray[i], &rfds);  // 将可用的fd设置到读文件描述符集中
                    if(_fdArray[i] > maxfd) maxfd = _fdArray[i];
                }
                // logMessage(DEBUG, "find maxfd success...");
                // logMessage(DEBUG, "befor select...");
                // timeout为nullptr：阻塞式等待 {0, 0}：非阻塞式等待 {5, 0}:5s内为阻塞时等待，之后为非阻塞式
                // struct timeval timeout = {1, 0};
                // int n = select(maxfd+1, &rfds, nullptr, nullptr, &timeout);
                int n = select(maxfd+1, &rfds, nullptr, nullptr, nullptr);
                // logMessage(DEBUG, "behind select, n = %d", n);
                switch(n)
                {
                case 0:
                    logMessage(NORMAL, "timeout..."); // 超时返回
                    break;
                case -1:
                    logMessage(WARNING, "select error, error: %d, err string: %s ", errno, strerror(errno));
                    break;
                default:
                    // 返回值 > 0 说明有事件就绪了 一开始什么事件都没有 只有listen
                    logMessage(NORMAL, "get a new link...");
                    HandlerReadEvents(rfds);
                    break;
                }

            }
        }

        ~SelectServer()
        {}

    private:
        int _port;
        int _listenSock;
        int *_fdArray;
        func_t _func;
    };
}