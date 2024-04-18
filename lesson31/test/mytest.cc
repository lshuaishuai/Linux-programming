#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define NUM 10

class ThreadData
{
public:
    int number;
    pthread_t tid;
    char namebuffer[64];
};

void *func(void *args)
{
    ThreadData *td = static_cast<ThreadData*>(args);

    cout << "我是新创建的线程,名字为" << td->namebuffer << " 编号为：" << td->number << endl;
    sleep(1);

    // 线程终止
    // 1
    return (void*)td->namebuffer;

}

int main()
{
    vector<ThreadData*> threads;
    // pthread_t tid;
    // 通过循环创建十个线程，使其执行一个函数
    for(int i = 0; i < NUM; i++)
    {
        ThreadData *td = new ThreadData;
        td->number = i + 1;
        // 写每个进程的名字
        snprintf(td->namebuffer, sizeof(td->namebuffer), "%s:%d", "thread", i);
        pthread_create(&td->tid, nullptr, func, td);
        // 将创建的十个线程都放到容器中
        threads.push_back(td);
    }

    // 线程取消 这里取消一半
    for(int i = 0; i < threads.size()/2; i++)
    {
        pthread_cancel(threads[i]->tid);
        cout << "cancel thread is " << threads[i]->number << endl;
    }

    // 线程等待
    for(auto& thread:threads)
    {
        void* retval;
        int n = pthread_join(thread->tid, &retval);
        assert(n == 0);
        cout << "join:" << thread->namebuffer << " success, retval:" << thread->namebuffer << endl;
        delete thread;
        thread = nullptr;
    }

    while(true)
    {
        cout << "我是主线程" << endl;
        sleep(1);
    }

    return 0;
}