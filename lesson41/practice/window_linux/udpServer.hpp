#pragma once

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

static const string defaultIp = "0.0.0.0";
static const int gNum = 1024;

enum {SOCKETERROR = 1, BINDERROR, USAGEERROR, RECVERROR};

class UdpServer
{
public:
    UdpServer() = default;

    UdpServer(const uint16_t& port, const string& ip = defaultIp)
        :_port(port)
        ,_ip(ip)
        ,_sockfd(-1)
    {}

    void initServer()
    {
        // 创建套接字
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(_sockfd == -1)
        {
            cerr << "socket error: " << errno << " " << strerror(errno) << endl;
            exit(SOCKETERROR);
        }
        cout << "socket success: " << _sockfd << endl;
    
        // 绑定套接字
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_port = htons(_port);
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = htonl(INADDR_ANY); // 接受任意要发给当前服务器信息机器发来的所有机器
        int n = bind(_sockfd, (struct sockaddr*)&local, sizeof((local)));
        if(n == -1)
        {
            cerr << "bind error: " << errno << " " << strerror(errno) << endl; 
            exit(BINDERROR);
        }
    }

    void start()
    {
        // 服务器就是死循环
        char buffer[gNum];
        for(;;)
        {
            // 接收数据
            struct sockaddr_in peer;  // 输出型参数 执行recvfrom会得到client的信息
            socklen_t len = sizeof(peer);
            ssize_t s = recvfrom(_sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&peer, &len);
            // 读取成功
            if(s > 0)
            {
                buffer[s] = 0;
                uint16_t clientPort = ntohs(peer.sin_port);
                string clientIp = inet_ntoa(peer.sin_addr); // 网络序列-》主机  数到点分十进制
                string message = buffer;
                cout << clientIp << "[" << clientPort << "]# " << message << endl;
                
                // 接收到数据以后 还可以进行一些操作
            }
            else if(s == -1) {
                cerr << "recv error: " << errno << " " <<strerror(errno) << endl;
                exit(RECVERROR); 
            }
        }
    }

private:
    int _sockfd;
    string _ip;
    uint16_t _port;
};