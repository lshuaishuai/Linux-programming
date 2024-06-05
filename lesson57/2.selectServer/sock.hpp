#pragma once

#include <iostream>    
#include <cstring>
#include <string>
#include <ctype.h>    
#include <sys/types.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h>    
#include <unistd.h> 

#include "log.hpp"
#include "err.hpp"


class Sock
{
    const static int gbacklog = 32;
public:
    static int Socket()
    {
        // 1.创建socket文件套接字对象    
        int sock = socket(AF_INET, SOCK_STREAM, 0); // 第二个参数与UDP不同    
        if (sock < 0)    
        {    
            // 创建套接字失败    
            logMessage(FATAL, "created socket error!");    
            exit(SOCKET_ERR);    
        }    
        logMessage(NORMAL, "created socket success: %d!", sock);    

        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        return sock;
    }

    static void Bind(int sock, int port)
    {
        // 2.bind绑定自己的网络信息
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));                                                                                                             
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            logMessage(FATAL, "bind socket error!");
            exit(BIND_ERR);
        }
        logMessage(NORMAL, "bind socket success!");
    }

    static void Listen(int sock)
    {
        // 3.设置socket 为监听状态
        if (listen(sock, gbacklog) < 0) // 第二个参数backlog后面会讲 5的倍数
        {
            logMessage(FATAL, "listen socket error!");
            exit(LISTEN_ERR);
        }
        logMessage(NORMAL, "listen socket success!");
    }

    static int Accept(int listensock, std::string *clientip, uint16_t *clientport)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sock = accept(listensock, (struct sockaddr *)&peer, &len); // sock 和client进行通信
        if (sock < 0) 
            logMessage(ERROR, "accept error, next!");
        else
        { 
            logMessage(NORMAL, "accept a new link success, get new sock: %d!", sock); // ?
            *clientip = inet_ntoa(peer.sin_addr);
            *clientport = ntohs(peer.sin_port);
        }
        return sock;
    }
};