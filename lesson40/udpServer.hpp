#pragma once

#include <iostream>
#include <string>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

namespace Server
{
    using namespace std;
    static const std::string defaultIp = "0.0.0.0";    
    static const int gnum = 1024;
    
    enum {USAGE_ERR = 1, SOCKET_ERR, BIND_ERR};

    typedef function<void (string, uint16_t, string)> func_t;
    
    class udpServer
    {
    public:
        udpServer() = default;

        udpServer(const func_t& cb, uint16_t &port, const std::string& ip = defaultIp)
            :_callback(cb)
            ,_port(port)
            ,_ip(ip)
            ,_sockfd(-1)
        {}

        void initServer()
        {
            // 1.创建socket
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM:UDP
            if(_sockfd == -1)
            {
                cerr << "socket error:" << errno << ":" << strerror(errno) << endl;
                exit(SOCKET_ERR);
            }
            cout << "socket success: " << ":" << _sockfd << endl;

            // 2.绑定port，ip(TODO)
            struct sockaddr_in local; // 就是定义了一个变量 栈上--用户
            bzero(&local, sizeof(local));
            local.sin_family = AF_INET; // 网络通信
            local.sin_port = htons(_port);  // 如果要给别人发消息 自己的ip和port也要发给对方 主机转为网络序列
            // local.sin_addr.s_addr = inet_addr(_ip.c_str()); // 1.将ip转为整数string->uint32_t 2.htonl(); -> 直接用系统接口
            local.sin_addr.s_addr = htonl(INADDR_ANY); // 任意地址bind 可以接受任意访问我这个端口的机器发来的数据
            int n = bind(_sockfd, (struct sockaddr*)&local, sizeof(local));
            if(n == -1)
            {
                cerr << "socket error:" << errno << ":" << strerror(errno) << endl;
                exit(BIND_ERR);
            }
            // UDP Server的预备工作完成
        }

        void start()
        {
            // 服务器的本质实际就是一个死循环 常驻内存的进程--最怕内存泄漏
            char buffer[gnum];
            for(;;)
            {
                // 读取数据
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer); // 必填
                ssize_t s = recvfrom(_sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&peer, &len);
                // 1.数据是什么？ 2.谁发的？
                if(s > 0)
                {
                    buffer[s] = 0;
                    string clientIp = inet_ntoa(peer.sin_addr); // 1.网络序列 2.int -> 点分十进制
                    uint16_t clientPort = ntohs(peer.sin_port);
                    string message = buffer;

                    cout << clientIp << "[" << clientPort << "]# " << message << endl; 
                    // 我们只把数据读上来就完了吗？ 对数据做处理
                    _callback(clientIp, clientPort, message);
                }
            }
        }

        ~udpServer(){}
    
    private:
        uint16_t _port; // 端口号 
        std::string _ip; // ip地址 实际上 一款网络服务器不建议指明一个IP
        int _sockfd; // 文件描述符
        func_t _callback; // 回调
    };
}