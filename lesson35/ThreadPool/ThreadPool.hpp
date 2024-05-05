#pragma once

# include "Thread.hpp"
# include "LockGuard.hpp"
# include <vector>
# include <queue>
# include <mutex>
# include <pthread.h>
# include <unistd.h>

using namespace ThreadNs;

const int pnum = 5;

template<class T>
class ThreadPool;

template <class T>
class ThreadData
{
public:
    ThreadPool<T> *threadpool;
    std::string name;
public:
    ThreadData(ThreadPool<T> *tp, const std::string& n)
        :threadpool(tp)
        ,name(n)
    { }
};

template<class T>
class ThreadPool
{
private:

    static void *handlerTask(void *args)
    {
        // 由于是静态函数 所以不能调用成员变量 需要靠参数获取当前的队列
        ThreadData<T> *td = static_cast<ThreadData<T>*>(args);
        while(true)
        {
            T t;
            // td->threadpool->lockQueue();
            {
                LockGuard lockguard(td->threadpool->mutex());
                while(td->threadpool->isQueueEmpty())
                {
                    td->threadpool->threadWait();
                }
                t = td->threadpool->pop(); // pop的本质 是从公共队列中拿到当前线程自己独立的栈中
            }
            // td->threadpool->unlockQueue(); 
            std::cout << td->name << " 获取了一个任务:" << t.toTaskString() << " 并处理完成，结果是：" << t() << std::endl; // 处理任务 要放在解锁之后 不然处理任务就变为串行了 而我们是要让不同进程并发的处理任务
        }
        delete td;
        return nullptr;
    }

public:

    void lockQueue() { pthread_mutex_lock(&_mutex); }
    void unlockQueue() { pthread_mutex_unlock(&_mutex); }
    bool isQueueEmpty() { return _task_queue.empty(); }
    void threadWait() { pthread_cond_wait(&_cond, &_mutex); }
    T pop()
    {
        T t = _task_queue.front();
        _task_queue.pop();
        return t;
    }

    pthread_mutex_t* mutex()
    {
        return &_mutex;
    }

    ThreadPool(const int &num = pnum)
        :_num(num)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_cond, nullptr);
        for(int i = 0; i < _num; i++)
        {
            _threads.push_back(new Thread());
        }
    }

    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(const ThreadPool&) = delete;

public:
  
    void run()
    {
        for(const auto& t : _threads)
        {
            ThreadData<T> *td = new ThreadData<T>(this, t->threadname());
            t->start(handlerTask, td);
            std::cout << t->threadname() << "start..." << std::endl;
        }
    }

    void Push(const T &in)
    {
        LockGuard lockguard(&_mutex);
        // pthread_mutex_lock(&_mutex);
        _task_queue.push(in);
        pthread_cond_signal(&_cond);
        // pthread_mutex_unlock(&_mutex);
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
        for(const auto &t : _threads) delete t;
    }

    static ThreadPool<T>* getInstance()
    {
        // 保证线程安全
        if(nullptr == tp)
        {
            _singLock.lock();
            if(nullptr == tp)
            {
                tp = new ThreadPool<T>();
            }
            _singLock.unlock();
        }
        return tp;
    }

private:
    int _num; // 线程池中线程的数量
    std::vector<Thread*> _threads;  // 管理一批线程
    std::queue<T> _task_queue;  // 任务队列
    pthread_mutex_t _mutex; // 锁
    pthread_cond_t _cond;  // 条件变量

    static ThreadPool<T> *tp; 
    static std::mutex _singLock;
};

template<class T>
ThreadPool<T>* ThreadPool<T>::tp = nullptr;

template<class T>
std::mutex ThreadPool<T>::_singLock;