# pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <functional>
#include <pthread.h>

class Thread;

// 上下文,当成一个大号的结构体
class Context
{
public:
    Thread *_this;
    void *_args;
public:

    Context()
    :_this(nullptr)
    ,_args(nullptr)
    {}

    ~Context()
    {}
};

class Thread
{
public:
    typedef std::function<void* (void*)> func_t;
    const int num = 1024;
public:
    Thread(func_t func, void *args, int number)
        :_func(func)
        ,_args(args)
    {
        char buffer[num];
        snprintf(buffer, sizeof(buffer), "thread-%d", number);
        _name = buffer;

        Context *ctx = new Context;
        ctx->_this = this;
        ctx->_args = _args;
        int n = pthread_create(&_tid, nullptr, start_routine, ctx);  //TODO
        // 意料之外用异常或if判断
        // assert 意料之中用assert
        assert(n == 0); // 编译debug的方式发布的时候存在，release方式发布，assert就不存在了，n是一个定义但是没有使用的变量，有的编译器会警告
        (void)n;
    }

    // 在类内创建线程，想让线程执行对应的方法，需要将方法设置为static static类型的无this指针
    static void *start_routine(void *args) // 类内成员，有缺省参数 其实是有两个参数的
    {
        Context *ctx = static_cast<Context*>(args);
        void *ret = ctx->_this->run(ctx->_args);

        delete ctx;
        return ret;
        // 静态方法只能调用静态的成员方法或者静态的成员变量
    }

    void join()
    {
        int n = pthread_join(_tid, nullptr);
        assert(n == 0);
        (void)n;
    }

    void *run(void *args)
    {
        return _func(args);
    }

    ~Thread()
    {

    }

private:
    std::string _name;
    pthread_t _tid;
    func_t _func;
    void *_args;
};