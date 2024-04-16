#include <iostream>
#include <cassert>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void func();

int val = 0;

// 新线程
void *thread_routine(void *args)
{
    const char *name = (const char*)args;
    while(1)
    {
        cout << "我是新线程, 名字为：" << name << ":";
        func();
        cout << "val:" << val++ << " &val:" << &val << endl;
        fflush(stdout);
        sleep(1);
    }
}

void func()
{
    cout << "我是一个独立的方法 ";
}

int main()
{
    pthread_t tid;
    int n = pthread_create(&tid, nullptr, thread_routine, (void*)"thread one");
    assert(n == 0);
    (void)n;

    while(1)
    {
        char tidbuffer[64];
        snprintf(tidbuffer, sizeof(tidbuffer), "0x%x", tid);
        
        cout << "我是主线程,我创建出来的线程id为:" << tidbuffer << ":" ;
        func();
        cout << "val:" << val << " &val:" << &val << endl;
        fflush(stdout);
        sleep(1);
    }

    return 0;
}