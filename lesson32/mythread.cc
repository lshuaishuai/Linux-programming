# include <iostream>
# include <cstdio>
# include <string>
# include <cstring>
# include <memory> 
# include <unistd.h>
# include <pthread.h>
# include "Thread.hpp"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

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

void *thread_run(void *args)
{
    std::string username = static_cast<const char*>(args);
    while(true)
    {
        pthread_mutex_lock(&lock);
        if(tickets > 0)
        {
            usleep(1234); // 1s=1000ms=1000 000us

            std::cout  << username << " 正在进行抢票:" << tickets << std::endl;
            // 用这段时间来模拟真实抢票花费的时间
            tickets--;
            pthread_mutex_unlock(&lock);
        }
        else
        {
            pthread_mutex_unlock(&lock);
            break;
        }        

        // 抢完票就完了吗？
    }
    return nullptr;
}

int main()
{
    std::unique_ptr<Thread> thread1(new Thread(thread_run, (void*)"helloThread", 0)); 
    std::unique_ptr<Thread> thread2(new Thread(thread_run, (void*)"countThread", 1)); 
    std::unique_ptr<Thread> thread3(new Thread(thread_run, (void*)"logThread", 2)); 

    thread1->join();
    thread2->join();
    thread3->join();

    return 0;
}