#pragma once 
#include <iostream>
#include <queue>
# include <pthread.h>

const int gmaxCap = 500;


template <class T>
class BlockQueue
{
public:

    BlockQueue(const int &maxCap = gmaxCap)
        :_maxCap(maxCap)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_consumerCond, nullptr);
        pthread_cond_init(&_productorCond, nullptr);
    }

    void Push(const T& in) // 输入型参数一般为const引用型
    {
        pthread_mutex_lock(&_mutex);
        // 细节2：充当条件判断的语法一定是while 不能是if ：当线程被唤醒后，由于是while 会返回来再进行一次判断 不啊毛南族条件才会继续执行 健壮性更高
        while(is_full())
        {
            // 1.细节1 pthread_cond_wait这个函数的第二个参数 必须是我们正在使用的互斥锁
            // a.pthread_cond_wait：该函数会以原子性的方式将锁释放，并将自己挂起
            // b.pthread_cond_wait：该函数在被唤醒返回的时候 会自动重新获取你传入的锁

            // 若此时缓冲队列是满的 则无法生产 生产者进行等待
            pthread_cond_wait(&_productorCond, &_mutex);
        }
        // 走到这里一定是不满的
        _q.push(in);
        // 阻塞队列一定有数据 则可以唤醒消费者来获取数据
        // 细节3：pthread_cond_signal：该函数可以放在临界区内部 也可以放在外部
        pthread_cond_signal(&_consumerCond); // 这里可以有一定的唤醒策略
        pthread_mutex_unlock(&_mutex);
    }

    void pop(T *out) // 输出型参数一般为*型  输入输出型一般为引用型
    {
        pthread_mutex_lock(&_mutex);
        
        while(is_empty())
        {
            // 若此时缓冲队列是空的 无法获取数据 消费者阻塞等待
            pthread_cond_wait(&_consumerCond, &_mutex);
        }

        // 走到这儿保证队列数据不空
        *out = _q.front();
        _q.pop();

        // 此时 决对能保证队列里至少有一个空位 唤醒生产者来生产
        pthread_cond_signal(&_productorCond);

        pthread_mutex_unlock(&_mutex);        
    }

    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_consumerCond);
        pthread_cond_destroy(&_productorCond);
    }

private:

    bool is_empty()
    {
        return _q.empty();
    }

    bool is_full()
    {
        return _q.size() == _maxCap;
    }

private:
    std::queue<T> _q; // 封装队列
    int _maxCap; // 队列最大容量
    pthread_mutex_t _mutex; // 把队列保护起来 STL不是线程安全的
    pthread_cond_t _productorCond; // 生产者对应的条件变量
    pthread_cond_t _consumerCond; // 消费者对应的条件变量
};