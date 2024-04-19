# include <iostream>
# include <cstdio>
# include <string>
# include <cstring>
# include <memory> 
# include <vector>
# include <unistd.h>
# include <pthread.h>
# include "Thread.hpp"
# include "Mutex.hpp"

#define NUM 4

// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// 共享资源
int tickets = 10000;

// 1.多个执行流进行安全访问的共享资源 - 临界资源
// 2.我们把对各执行流中，访问临界资源的代码 -- 临界区
// 3.为了安全 想让多个线程串行访问共享资源 -- 互斥
// 4.对一个资源进行访问的时候，要么不做，要么做完 -- 原子性， 不是原子性的情况 -- 一个对资源进行的操作，如果只用一条汇编语句就能完成就是原子的，否则不是！

// 解决方案 加锁

// 需要尽可能的让多个线程交叉执行
// 本质：让调度器尽可能地频繁发生线程调度与切换
// 线程一般在什么时候发生切换？时间片到了/来了更高优先级的线程/或者线程等待的时候
// 线程是在什么时候检测上面的问题呢？从内核态返回用户态的时候，线程对调度状态进行检测，如果可以，就直接发生线程切换

class ThreadData
{
public:

    ThreadData(const std::string &threadname, pthread_mutex_t *mutex_p)
        :_threadname(threadname)
        ,_mutex_p(mutex_p)
    {}

    ~ThreadData(){}

public:
    std::string _threadname;
    pthread_mutex_t *_mutex_p;
};

void *thread_run(void *args)
{
    // std::string username = static_cast<const char*>(args);
    ThreadData *td = static_cast<ThreadData*>(args);
    while(true)
    {
        // 加锁和解锁的过程是由多个线程串行执行的 程序会变慢
        // 锁之规定互斥访问，并没有规定不许让谁优先执行
        // pthread_mutex_lock(td->_mutex_p);
        {
            LockGuard lockguard(td->_mutex_p);
            if(tickets > 0)
            {
                usleep(1234); // 1s=1000ms=1000 000us
                // std::cout  << username << " 正在进行抢票:" << tickets << std::endl;

                std::cout  << td->_threadname << " 正在进行抢票:" << tickets << std::endl;
                // 用这段时间来模拟真实抢票花费的时间
                tickets--;
                // pthread_mutex_unlock(td->_mutex_p);
            }
            else 
            {
                // pthread_mutex_unlock(td->_mutex_p);
                break; 
            }  
        }        

        // 抢完票就完了吗？ 模拟抢完票之后的流程
        usleep(1000);
    }
    return nullptr;
}

int main()
{
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, nullptr);
    std::vector<pthread_t> tids(NUM);
    for(int i = 1; i <= NUM; i++)
    {
        char namebuffer[64];
        snprintf(namebuffer, sizeof(namebuffer), "thread %d", i);
        ThreadData *td = new ThreadData(namebuffer, &lock);
        pthread_create(&tids[i-1], nullptr, thread_run, td);
    }

    for(const auto &tid:tids)
    {
        pthread_join(tid, nullptr);
    }

    
    // pthread_t t1, t2, t3, t4;
    // pthread_create(&t1, nullptr, thread_run, (void*)"thread 1");
    // pthread_create(&t2, nullptr, thread_run, (void*)"thread 2");
    // pthread_create(&t3, nullptr, thread_run, (void*)"thread 3");
    // pthread_create(&t4, nullptr, thread_run, (void*)"thread 4");
    
    // pthread_join(t1, nullptr);
    // pthread_join(t2, nullptr);
    // pthread_join(t3, nullptr);
    // pthread_join(t4, nullptr);

    pthread_mutex_destroy(&lock);


    // std::unique_ptr<Thread> thread1(new Thread(thread_run, (void*)"helloThread", 0)); 
    // std::unique_ptr<Thread> thread2(new Thread(thread_run, (void*)"countThread", 1)); 
    // std::unique_ptr<Thread> thread3(new Thread(thread_run, (void*)"logThread", 2)); 

    // thread1->join();
    // thread2->join();
    // thread3->join();

    return 0;
}