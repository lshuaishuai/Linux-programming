#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <cassert>

#include "Log.hpp"
#include "Err.hpp"
#include "Sock.hpp"
#include "Epoller.hpp"
#include "Utils.hpp"

namespace tcpserver
{
    static const uint16_t defaultport = 8080;

    class Connection;
    class TcpServer;

    using func_t = std::function<void (Connection*)>;

    class Connection
    {
    public:
        Connection(int sock)
            :_sock(sock)
            ,_tsp(nullptr)
        {}
        
        void Register(func_t r, func_t s, func_t e)
        {
            _recver = r;
            _sender = s;
            _excepter = e;
        }   

        ~Connection()
        {}

    public:
        int _sock;
        // 每个sock都需要自己的缓冲区
        std::string _inbuffer;  // 输入缓冲区
        std::string _outbuffer; // 输出缓冲区

        func_t _recver;   // 从_sock中读
        func_t _sender;   // 向_sock中写
        func_t _excepter; // 处理_sock IO的时候上面的异常事件

        TcpServer* _tsp; // ？？可以省略
    };

    class TcpServer
    {
    public:
        TcpServer(uint16_t port = defaultport)
            :_port(port)
        {}

        void InitServer()
        {
            // 1.创建socket
            _sock.Socket();
            _sock.Bind(_port);
            _sock.Listen();

            // 2.构建Epoller对象
            _epoller.Create();

            // 3.将目前唯一的sock添加到epoller中，在这之前将对应的fd设置为非阻塞的
            // listensock也是socket，也要看成一个Connection
            AddConnection(_sock.Fd(), EPOLLIN | EPOLLET, std::bind(&TcpServer::Accepter, this, std::placeholders::_1), nullptr, nullptr);

        }

        void Accepter(Connection* conn)
        {

        }

        void AddConnection(int sock, uint32_t events, func_t recver, func_t sender, func_t excepter)
        {
            // 1.首先为sock创建Connection，并初始化，添加带map中
            if(events & EPOLLIN) Util::SetNonBlock(_sock.Fd());
            Connection *conn = new Connection(sock);
            // 2.给对应的sock设置对应的回调方法
            conn->Register(recver, sender, excepter);

            // 2.其次将sock与它要关心的事件"写透式"注册到epoll中华，染让epoll帮我们关心.
            bool r = _epoller.AddEvent(sock, events); // ET模式下的文件描述应该为非阻塞的
            assert(r);
            (void)r;

            // 3.将kv添加到map中
            _connections.insert(make_pair(sock, conn));
        }

        

        void Start()
        {

        }

        ~TcpServer()
        {}
    private:
        uint16_t _port;
        Epoller _epoller; 
        Sock _sock;
        std::unordered_map<int, Connection*> _connections;
    };
}