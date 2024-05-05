#pragma once

#include <iostream>
#include <string>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

namespace Client
{
    using namespace std;
    class udpClient
    {
    public:
        udpClient(const string& serverIp, const uint16_t& serverPort)
            :_sockfd(-1)
            ,_serverIp(serverIp)
            ,_quit(false)
            ,_serverPort(serverPort)
        {}

        void initClient()
        {
            // 1.创建socket
            _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if(_sockfd == -1)
            {
                cerr << "socket error: " << errno << strerror(errno) << endl;
                exit(1);
            }
            cout << "socket success: " << ":" << _sockfd << endl;

            // 2.client需要bind吗?--需要 client需要显式的bind吗(程序员自己bind)？--不需要 由OS自动形成进行bind！ 
            // -- OS在什么时候、如何bind

        }

        void run()
        {
            struct sockaddr_in server;
            memset(&server, 0, sizeof(server)); 
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = inet_addr(_serverIp.c_str());
            server.sin_port = htons(_serverPort);

            string message;
            while(!_quit)
            {
                cout << "Please Enter# ";
                cin >> message;

                sendto(_sockfd, message.c_str(), message.size(), 0, (struct sockaddr*)&server, sizeof(server));
            }
        }

        ~udpClient(){}
    private:
        int _sockfd;
        string _serverIp;
        uint16_t _serverPort;
        bool _quit;
    };
}