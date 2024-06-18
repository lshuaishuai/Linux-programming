#pragma once

#include <iostream>    
#include <cstring>
#include <string>
#include <ctype.h>  
#include <poll.h>  
#include <sys/types.h>    
#include <sys/socket.h>    
#include <netinet/in.h>    
#include <arpa/inet.h>    
#include <unistd.h> 

#include "Log.hpp"
#include "Err.hpp"

const static int gbacklog = 32;
const static int defaultsock = -1;

class Sock
{
public:
    Sock()
        :_listensock(defaultsock)
    {}

    ~Sock()
    {
        if(_listensock != defaultsock) close(_listensock);
    }

public:
    void Socket()
    {
        // 1.创建socket文件套接字对象    
        _listensock = socket(AF_INET, SOCK_STREAM, 0); // 第二个参数与UDP不同    
        if (_listensock < 0)    
        {    
            // 创建套接字失败    
            logMessage(FATAL, "created socket error!");    
            exit(SOCKET_ERR);    
        }    
        logMessage(NORMAL, "created socket success: %d!", _listensock);    

        int opt = 1;
        setsockopt(_listensock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    }

    void Bind(int port)
    {
        // 2.bind绑定自己的网络信息
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));                                                                                                             
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        if (bind(_listensock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            logMessage(FATAL, "bind socket error!");
            exit(BIND_ERR);
        }
        logMessage(NORMAL, "bind socket success!");
    }

    void Listen()
    {
        // 3.设置socket 为监听状态
        if (listen(_listensock, gbacklog) < 0) // 第二个参数backlog后面会讲 5的倍数
        {
            logMessage(FATAL, "listen socket error!");
            exit(LISTEN_ERR);
        }
        logMessage(NORMAL, "listen socket success!");
    }

    int Accept(std::string *clientip, uint16_t *clientport, int* err)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int sock = accept(_listensock, (struct sockaddr *)&peer, &len); // sock 和client进行通信
        *err = errno;
        if (sock < 0) {}
            // logMessage(ERROR, "accept error, next!");
        else
        { 
            // logMessage(NORMAL, "accept a new link success, get new sock: %d!", sock); // ?
            *clientip = inet_ntoa(peer.sin_addr);
            *clientport = ntohs(peer.sin_port);
        }
        return sock;
    }

    int Fd()
    {
        return _listensock;
    }

    void Close()
    {
        if(_listensock != defaultsock) close(_listensock);
    }
    
private:
    int _listensock;
};