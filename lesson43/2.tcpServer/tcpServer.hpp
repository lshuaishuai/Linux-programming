#pragma once

#include "log.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

namespace server
{
    using namespace std;

    enum {
        USAGE_ERR = 1,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR
    };

    static const uint16_t gport = 8080;
    static const int gbacklog = 5;

    class TcpServer;
    class ThreadData
    {
    public:
        ThreadData(TcpServer *self, int sock)
            :_self(self)
            ,_sock(sock)
        {}

        TcpServer *_self;
        int _sock;
    };

    class TcpServer
    {
    public:

        TcpServer(const uint16_t& port = gport)
            :_port(port)
            ,_listenSockfd(-1)
        {}

        void initServer()
        {
            // 1.创建socket文件套接字对象
            _listenSockfd = socket(AF_INET, SOCK_STREAM, 0); // 第二个参数与UDP不同
            if(_listenSockfd < 0)
            {
                // 创建套接字失败
                logMessage(FATAL, "created socket error!");
                exit(SOCKET_ERR);
            }
            logMessage(NORMAL, "created socket success: %d!", _listenSockfd);

            // 2.bind绑定自己的网络信息
            struct sockaddr_in local;
            memset(&local, 0, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(_port);
            local.sin_addr.s_addr = INADDR_ANY;
            if(bind(_listenSockfd, (struct sockaddr*)&local, sizeof(local)) < 0)
            {
                logMessage(FATAL, "bind socket error!");
                exit(BIND_ERR);
            }
            logMessage(NORMAL, "bind socket success!");

            // 3.设置socket 为监听状态
            if(listen(_listenSockfd, gbacklog) < 0) // 第二个参数backlog后面会讲 5的倍数
            {
                logMessage(FATAL, "listen socket error!");
                exit(LISTEN_ERR);
            }
            logMessage(NORMAL, "listen socket success!");

        }

        void start()
        {
            // 线程池初始化
            ThreadPool<Task>::getInstance()->run();  // 获得单例
            logMessage(NORMAL, "Thread init success!");

            signal(SIGCHLD, SIG_IGN);
            for( ; ; )
            {
                // 4.server 获取新连接 不能直接接收数据/发送数据
                struct sockaddr_in peer;
                socklen_t len = sizeof(peer);
                int sock = accept(_listenSockfd, (struct sockaddr*)&peer, &len); //sock 和client进行通信
                if(sock < 0)
                {
                    logMessage(ERROR, "accept error, next!");
                    continue;
                }
                logMessage(NORMAL, "accept a new link success, get new sock: %d!", sock); // ?
                

                // 5.这里就是一个sock， 未来通信通sock，面向字节流的，后续全部都是文件操作！
                
                // verson1
                // serviceIO(sock);
                // close(sock); // 对一个已经使用完毕的sock 要关闭这个sock 要不然会导致文件描述符泄露

                // version 2, 多进程版，
                // pid_t id = fork();
                // if(id == 0)
                // {
                //     // 子进程 向外提供服务 不需要监听 关闭这个文件描述符
                //     close(_listenSockfd);
                //     // if(fork() > 0) exit(0); // 让子进程的子进程执行下面代码 子进程退出
                //     serviceIO(sock);
                //     close(sock); // 关闭父进程的
                //     exit(0);  // 最后变成孤儿进程 交给OS回收这个进程
                // }
                // close(sock); // 关闭子进程的

                // 父进程
                // pid_t ret = waitpid(id, nullptr, 0); // 阻塞式等待
                // if(ret > 0)
                // {
                //     cout << "wait success: " << ret << endl;
                // }

                // version3 多线程版， 有频繁创建的问题
                // pthread_t tid;
                // ThreadData* td = new ThreadData(this, sock);
                // pthread_create(&tid, nullptr, threadRoutine, td); // 每个线程不需要关闭文件描述符
                
                // // pthread_join(tid, nullptr); // 阻塞等待，不可行 使用线程分离

                // version4 线程池版
                
                ThreadPool<Task>::getInstance()->Push(Task(sock, serviceIO));
            }
        }

        static void *threadRoutine(void *args) // 必须为
        {
            pthread_detach(pthread_self());
            ThreadData* td = static_cast<ThreadData*>(args);
            serviceIO(td->_sock);
            delete td;
            close(td->_sock);
            return nullptr;
        }
        
        ~TcpServer()
        {}
    private:
        int _listenSockfd; // 套接字 -- 不是用来通信的 是用来监听链接到来，获取新链接的！
        uint16_t _port; // 端口号
    };
}