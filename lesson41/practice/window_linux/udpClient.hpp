#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

enum {SOCK_ERR = 1, USAGE_ERROR};
static const int gNUm = 1024;

class UdpClient
{
public:
    UdpClient() = default;

    UdpClient(const string& serverIp, const uint16_t& serverPort)
        :_serverIp(serverIp)
        ,_serverPort(serverPort)
        ,_sockfd(-1)
    {}

    void initClient()
    {
        // 客户端只需要自己创建套接字 绑定OS自己完成
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(_sockfd == -1)
        {
            cerr << "socket error: " << errno << " " << strerror(errno) << endl;
            exit(SOCK_ERR);
        }
        cout << "socket success: " << _sockfd << endl;
    }

    void runClient()
    {
        // 发送数据 需要的话接受服务器发回的数据
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(_serverPort);
        server.sin_addr.s_addr = inet_addr(_serverIp.c_str());
        char buffer[gNUm];
        while(true)
        {
            cout << "pleaseEnter# ";
            cin >> buffer;
            ssize_t s = sendto(_sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&server, sizeof(server));
        }
    }

private:
    int _sockfd;
    string _serverIp;
    uint16_t _serverPort;

};