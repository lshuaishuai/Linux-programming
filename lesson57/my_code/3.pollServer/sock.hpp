#pragma once

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
        int listensock = socket(AF_INET, SOCK_STREAM, 0);
        if (listensock < 0)
        {
            logMessage(FATAL, "created socket error!");
            exit(SOCKET_ERR);
        }
        logMessage(NORMAL, "created socket success, listen sock: %d ", listensock);

        int opt = 1;
        setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        return listensock;
    }

    static void Bind(int sock, uint16_t port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_port = htons(port);
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = INADDR_ANY;

        int n = bind(sock, (sockaddr *)&local, sizeof(local));
        if (n < 0)
        {
            logMessage(FATAL, "bind socket error!");
            exit(BIND_ERR);
        }
        logMessage(NORMAL, "bind socket success!");
    }

    static void Listen(int listensock)
    {
        int n = listen(listensock, gbacklog);
        if (n < 0)
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
        int sock = accept(listensock, (sockaddr *)&peer, &len);

        if (sock < 0)
        {
            logMessage(ERROR, "accept error, next!");
        }
        else
        {
            logMessage(NORMAL, "accept a new link success, get new sock: %d", sock);
            *clientip = inet_ntoa(peer.sin_addr);
            *clientport = ntohs(peer.sin_port);
        }
        return sock;
    }
};