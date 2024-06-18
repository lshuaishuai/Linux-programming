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
#include "Protocol.hpp"

namespace tcpserver
{
    static const uint16_t defaultport = 8080;
    static const int num = 64;

    class Connection;
    class TcpServer;

    using func_t = std::function<void(Connection *)>;
    // using hander_t = std::function<void(const std::string& package)>;

    class Connection
    {
    public:
        Connection(int sock)
            : _sock(sock), _tsp(nullptr)
        {
        }

        void Register(func_t r, func_t s, func_t e)
        {
            _recver = r;
            _sender = s;
            _excepter = e;
        }

        ~Connection()
        {
        }

    public:
        int _sock;
        // 每个sock都需要自己的缓冲区
        std::string _inbuffer;  // 输入缓冲区
        std::string _outbuffer; // 输出缓冲区

        func_t _recver;   // 从_sock中读
        func_t _sender;   // 向_sock中写
        func_t _excepter; // 处理_sock IO的时候上面的异常事件

        TcpServer *_tsp; // ？？可以省略
    };

    class TcpServer
    {
    public:
        TcpServer(func_t func, uint16_t port = defaultport)
            : _service(func)
            , _port(port)
            , _revs(nullptr)
        {
        }

        void InitServer()
        {
            // 1.创建socket
            _sock.Socket();
            _sock.Bind(_port);
            _sock.Listen();

            // 2.构建Epoller对象
            _epoller.Create();

            // 3.将目前唯一的sock添加到epoller中，在这之前将对应的fd设置为非阻塞的
            // listensock也是socket，也要看成一个Connection  只关心读事件
            AddConnection(_sock.Fd(), EPOLLIN | EPOLLET, std::bind(&TcpServer::Accepter, this, std::placeholders::_1), nullptr, nullptr);
            _revs = new struct epoll_event[num];
            _num = num;
        }

        // 事件派发器
        void Dispatch()
        {
            int timeout = -1;
            while (true)
            {
                Loop(timeout);
                // logMessage(DEBUG, "timeout...");
            }
        }

        ~TcpServer()
        {
            _sock.Close();
            _epoller.Close();
            if (_revs != nullptr)
                delete[] _revs;
        }

    private:
        void Recver(Connection *conn)
        {
            // Util::SetNonBlock(conn->_sock);
            char buffer[1024];
            while (true)
            {
                // 这里的conn->_sock必须为非阻塞的
                ssize_t s = recv(conn->_sock, buffer, sizeof(buffer) - 1, 0);
                if (s > 0)
                {
                    buffer[s] = 0;
                    conn->_inbuffer += std::string(buffer); // 将读到的数据放入sock自己的缓冲区
                }
                else if (s == 0)
                {
                    if (conn->_excepter)
                    {
                        conn->_excepter(conn);
                    }
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        break; // 是因为底层无数据
                    else if (errno == EINTR)
                        continue; // 被信号中断
                    else
                    {
                        if (conn->_excepter)
                        {
                            conn->_excepter(conn);
                            break;
                        }
                    }
                }
            }
            
            _service(conn);
        }

        void Sender(Connection *conn)
        {
            while(true)
            {
                ssize_t s = send(conn->_sock, conn->_outbuffer.c_str(), conn->_outbuffer.size(), 0);
                if(s > 0)
                {
                    if(conn->_outbuffer.empty()) break; // TODO
                    else
                        conn->_outbuffer.erase(0, s); // 下次发的就是没有发的
                }
                else
                {
                    if(errno == EAGAIN || errno == EWOULDBLOCK) break;
                    else if(errno == EINTR) continue;
                    else
                    {
                        if (conn->_excepter)
                        {
                            conn->_excepter(conn);
                            break;
                        }
                    }
                }
            }
        }

        void Excepter(Connection *conn)
        {
            // 统一对所有异常进行处理
        }

        void Print(int readyNum)
        {
            std::cout << "fd list: ";
            for (int i = 0; i < readyNum; i++)
            {
                std::cout << _revs[i].data.fd << " ";
            }
            std::cout << std::endl;
        }

        void Accepter(Connection *conn)
        {
            for (;;)
            {
                std::string clientip;
                uint16_t clientport;
                int err = 0;
                int sock = _sock.Accept(&clientip, &clientport, &err);

                if (sock > 0)
                {
                    AddConnection(sock, EPOLLIN | EPOLLET,
                                  std::bind(&TcpServer::Recver, this, placeholders::_1),
                                  std::bind(&TcpServer::Sender, this, placeholders::_1),
                                  std::bind(&TcpServer::Excepter, this, placeholders::_1));
                    logMessage(DEBUG, "get a new link, info: [%s:%d]", clientip.c_str(), clientport);
                }
                else
                {
                    if (err == EAGAIN || err == EWOULDBLOCK)
                        break;
                    else if (err == EINTR)
                        continue; // 被信号中断
                    else
                        break;
                }
            }
        }

        void AddConnection(int sock, uint32_t events, func_t recver, func_t sender, func_t excepter)
        {
            // 1.首先为sock创建Connection，并初始化，添加带map中
            if (events & EPOLLIN)
                Util::SetNonBlock(sock);
            Connection *conn = new Connection(sock);
            // 2.给对应的sock设置对应的回调方法
            conn->Register(recver, sender, excepter);
            // 2.其次将sock与它要关心的事件"写透式"注册到epoll中华，染让epoll帮我们关心.
            bool r = _epoller.AddEvent(sock, events); // ET模式下的文件描述应该为非阻塞的
            assert(r);
            (void)r;

            // 3.将kv添加到map中
            _connections.insert(make_pair(sock, conn));

            logMessage(DEBUG, "add new sock: %d in epoll and unordered_map!", sock);
        }

        bool isConnectionExists(int sock)
        {
            auto iter = _connections.find(sock);
            return iter != _connections.end();
        }

        void Loop(int timeout)
        {
            int n = _epoller.Wait(_revs, _num, timeout); // 获取已经就虚的事件
            for (int i = 0; i < n; i++)
            {
                int sock = _revs[i].data.fd;
                uint32_t events = _revs[i].events;

                // 若有事件异常，则将其全部转化，成为读写问题
                if (events & EPOLLERR)
                    events |= (EPOLLIN | EPOLLOUT);
                if (events & EPOLLHUP)
                    events |= (EPOLLIN | EPOLLOUT);

                // if((events & EPOLLIN) && sock == _sock.Fd()) // 监听事件就绪
                //     _connections[sock]->_recver(_connections[sock]);
                // if((events & EPOLLIN) && sock != _sock.Fd()) // 普通文件描述符
                //     _connections[sock]->_recver(_connections[sock]);
                if ((events & EPOLLIN) && isConnectionExists(sock) && _connections[sock]->_recver)
                {
                    _connections[sock]->_recver(_connections[sock]);
                }
                if ((events & EPOLLOUT) && isConnectionExists(sock) && _connections[sock]->_sender)
                {
                    _connections[sock]->_sender(_connections[sock]);
                }
            }
        }

    private:
        uint16_t _port;
        Epoller _epoller;
        Sock _sock;
        std::unordered_map<int, Connection *> _connections;
        struct epoll_event *_revs;
        int _num;
        // hander_t _hander;
        func_t _service;
    };
}