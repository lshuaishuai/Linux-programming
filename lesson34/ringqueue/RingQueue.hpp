#pragma once 

# include <iostream>
# include <vector>
# include <cassert>
# include <semaphore.h>
# include <pthread.h>

static const int gcap = 100;

template <class T>
class RingQueue
{
private:
    void P(sem_t &sem)
    {
        int n = sem_wait(&sem);
        assert(n == 0);
        (void)n;
    }

    void V(sem_t &sem)
    {
        int n = sem_post(&sem);
        assert(n == 0);
        (void)n;
    }

public:
    RingQueue(const int &cap = gcap)
        :_queue(gcap)
        ,_cap(cap)
    {
        int n = sem_init(&_p_sapceSem, 0, _cap);
        assert(n == 0);
        n = sem_init(&_c_dataSem, 0, 0);
        assert(n == 0);
        _producttorStep = _consumerStep = 0;

        pthread_mutex_init(&_pmutex, nullptr);
        pthread_mutex_init(&_cmutex, nullptr);
    }

    void Push(const T &in)
    {
        // 加锁和申请信号量 谁先谁后比较合适
        P(_p_sapceSem); // 申请到了空间信号量 意味着一定能进行正常的生产
        pthread_mutex_lock(&_pmutex);
        _queue[_producttorStep++] = in;
        _producttorStep %= _cap;
        pthread_mutex_unlock(&_pmutex);
        V(_c_dataSem);
    }

    void Pop(T *out)
    {
        P(_c_dataSem);
        pthread_mutex_lock(&_cmutex);
        *out = _queue[_consumerStep++];
        _consumerStep %= _cap;
        pthread_mutex_unlock(&_cmutex);
        V(_p_sapceSem);
    }

    ~RingQueue()
    {
        sem_destroy(&_p_sapceSem);
        sem_destroy(&_c_dataSem);

        pthread_mutex_destroy(&_pmutex);
        pthread_mutex_destroy(&_cmutex);
    }

private:
    std::vector<T> _queue;
    int _cap; // 环形队列容量
    sem_t _p_sapceSem; // 生产者 看中的是空间资源
    sem_t _c_dataSem; // 消费者 看中的是数据资源
    int _producttorStep; // 生产者位置
    int _consumerStep; // 消费者位置

    // 为了实现多消费多生产 需要加两把锁
    pthread_mutex_t _pmutex;
    pthread_mutex_t _cmutex;
};
