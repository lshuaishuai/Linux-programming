#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "protocol.hpp"

using namespace std;

#define NUM 1024


class CalClient
{
public:
    CalClient(const string& serverIp, const uint16_t& serverPort)
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
            string line;
            std::string inbuffer;
            while(true)
            {
                cout << "Mycal>>> ";
                getline(cin, line);
                
    
                Request req = ParseLine(line);
                std::string content;
                req.serialize(&content); // 序列化
                std::string send_string = enLength(content);
                send(_sock, send_string.c_str(), send_string.size(), 0); // BUG?

                std::string package, text;
                if(!recvPackge(_sock, inbuffer, &package)) continue;
                if(!deLength(package, &text)) continue;
                Response resp;
                resp.deserialize(text);
                std::cout << "exitCode: " << resp._exitcode << std::endl;
                std::cout << "result: " << resp._result << std::endl;
            }
        }
    }

    const Request& ParseLine(const std::string& line)
    {
        // "1+1" "123*123" "21/0"
        int status = 0; // 0:开始 1：碰到操作符 2：操作符之后
        int i = 0;
        int cnt = line.size();
        std::string left, right; // 左右操作数
        char op;
        while(i < cnt)
        {
            switch(status)
            {
            case 0:
            {
                if(!isdigit(line[i])) 
                {
                    op = line[i];
                    status = 1;
                }
                else left.push_back(line[i++]);
            }
            break;  
            case 1:
                i++;
                status = 2;
                break;
            case 2:
                right.push_back(line[i++]); 
                break;
            }
        }
        cout << std::stoi(left) << op << std::stoi(right) << std::endl;
        return Request(std::stoi(left), std::stoi(right), op);
    }

    ~CalClient()
    {
        if(_sock >= 0) close(_sock);
    }

private:
    int _sock;
    string _serverIp;
    uint16_t _serverPort;
};