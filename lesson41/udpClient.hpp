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
            // -- OS在什么时候、如何bind 客户端的端口号是多少不重要，唯一性才重要

        }

        static void* recvMessage(void *args)
        {
            int sockfd = *(static_cast<int*>(args));
            pthread_detach(pthread_self()); // 线程分离
            while(1)
            {
                char buffer[1024];
                struct sockaddr_in temp;
                socklen_t temp_len = sizeof(temp);
                size_t n = recvfrom(sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&temp, &temp_len);
                if(n > 0) buffer[n] = 0;
                cout << buffer << endl;
            }
            return nullptr;
        }

        void run()
        {
            // 多线程方式 主线程发 新创建线程的收信息
            pthread_create(&_reader, nullptr, recvMessage, (void*)&_sockfd);

            struct sockaddr_in server;
            memset(&server, 0, sizeof(server)); 
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = inet_addr(_serverIp.c_str());
            server.sin_port = htons(_serverPort);

            char message[1024];
            while(!_quit)
            {
                // cout << "Please Enter# ";
                char print[1024];
                fprintf(stderr, "Enter# ");
                cin.getline(message, sizeof(message));

                sendto(_sockfd, message, sizeof(message), 0, (struct sockaddr*)&server, sizeof(server));
                
            }
        }

        ~udpClient(){}
    private:
        int _sockfd;
        string _serverIp;
        uint16_t _serverPort;
        bool _quit;

        pthread_t _reader;
    };
}