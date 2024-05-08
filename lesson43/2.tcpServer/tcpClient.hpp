#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define NUM 1024


class TcpClient
{
public:
    TcpClient(const string& serverIp, const uint16_t& serverPort)
        :_sock(-1)
        ,_serverIp(serverIp)
        ,_serverPort(serverPort)
    {}

    void initClient()
    {
        // 1.创建socket
        _sock = socket(AF_INET, SOCK_STREAM, 0);
        if(_sock < 0)
        {
            cerr << "socket creat error!" << endl;
            exit(2);
        }
        // 2.TCP的客户端要bind 但不需要显式的bind，OS自动完成

        // 3.要不要listen？ 不要
        // 4.要不要accept？ 不要
        // 5.要发起链接
    }

    void start()
    {
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(_serverPort);
        server.sin_addr.s_addr = inet_addr(_serverIp.c_str());
       
        if(connect(_sock, (struct sockaddr*)&server, sizeof(server)) != 0)
        {
            cerr << "socket connect error" << endl;

        }
        else
        {
            string msg;
            while(true)
            {
                cout << "Enter# ";
                getline(cin, msg);

                write(_sock, msg.c_str(), msg.size());

                char buffer[NUM];
                int n = read(_sock, buffer, sizeof(buffer)-1);
                if(n > 0)
                {
                    // 当成字符串
                    buffer[n] = 0;
                    cout << "Server 回显# " << buffer << endl;
                }
                else
                {
                    break;
                }
            }
        }
    }

    ~TcpClient()
    {
        if(_sock >= 0) close(_sock);
    }

private:
    int _sock;
    string _serverIp;
    uint16_t _serverPort;
};