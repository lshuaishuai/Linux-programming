#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.hpp"
#include "err.hpp"

class Sock
{
    const static int gbacklog = 32;

public:
    static int Socket()
    {
        int listenSock = socket(AF_INET, SOCK_STREAM, 0);
        if(listenSock < 0)
        {
            logMessage(FATAL, "created socket error!");
            exit(SOCKER_ERR);
        }
        logMessage(NORMAL, "created socket success: %d", listenSock);
        int opt = 1;
        setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)); // 这个函数不太懂 下来好好看看
        
        return listenSock;
    }

    static void Bind(int sock, uint16_t port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));  // 这行代码使用memset函数将local变量的内存空间初始化为0。这是为了确保所有的字段都被正确设置。
        local.sin_port = htons(port);  // 这行代码将指定的端口号(port)转换为网络字节顺序（big-endian）
        local.sin_family = AF_INET;  // 这行代码将AF_INET赋值给local变量的sin_family字段。sin_family用于指定地址族，AF_INET表示IPv4地址
        local.sin_addr.s_addr = INADDR_ANY;  // INADDR_ANY表示接受任意IP地址，用于服务器端绑定到所有可用的网络接口

        int n = bind(sock, (sockaddr*)&local, sizeof(local));
        if(n == 0)
        {
            logMessage(NORMAL, "bind socket success!");
        }
        else{
            logMessage(FATAL, "bind socket error!");
            exit(BIND_ERR);
        }
    }

    static void Listen(int sock)
    {
        int n = listen(sock, gbacklog);
        if(n < 0)
        {
            logMessage(FATAL, "listen socket error!");
            exit(LISTEN_ERR);
        }
        logMessage(NORMAL, "listen socket success!");
    }

    static int Accept(int listenSock, string *clientip, uint16_t *clientport)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sock = accept(listenSock, (sockaddr*)&peer, &len);
        if(sock < 0)
            logMessage(ERROR, "accept error, next!");
        else{
            logMessage(NORMAL, "accept a new link success, get new sock: %d ", sock);
            *clientip = inet_ntoa(peer.sin_addr);
            *clientport = ntohs(peer.sin_port);
        }
        return sock;
    }
};