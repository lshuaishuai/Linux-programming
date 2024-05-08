# pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <functional>
#include <pthread.h>

namespace ThreadNs
{
    typedef std::function<void* (void*)> func_t;
    const int num = 1024;

    class Thread
    {
    private:
    
        // 在类内创建线程，想让线程执行对应的方法，需要将方法设置为static static类型的无this指针
        static void *start_routine(void *args) // 类内成员，有缺省参数 其实是有两个参数的
        {
            Thread *thread = static_cast<Thread*>(args);
            return thread->callBack();
        }

    public:
        Thread()
        {
            char namebuffer[num];
            snprintf(namebuffer, sizeof(namebuffer), "thread-%d", _threadNum++);
            _name = namebuffer;
        }

        void start(func_t func, void *args = nullptr)
        {
            _func = func;
            _args = args;
            int n = pthread_create(&_tid, nullptr, start_routine, this);  //TODO
            assert(n == 0); // 编译debug的方式发布的时候存在，release方式发布，assert就不存在了，n是一个定义但是没有使用的变量，有的编译器会警告
            (void)n;
        }
        
        void join()
        {
            int n = pthread_join(_tid, nullptr);
            assert(n == 0);
            (void)n;
        }

        void *callBack()
        {
            return _func(_args);
        }

        std::string threadname()
        {
            return _name;
        }

        ~Thread()
        {

        }

    private:
        std::string _name;
        pthread_t _tid;
        func_t _func;
        void *_args;

        static int _threadNum;
    };
    int Thread::_threadNum = 1;
}