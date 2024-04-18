// #include <iostream>
// #include <thread>
// #include <unistd.h>

// using namespace std;

// void thread_run()
// {
//     while(true)
//     {
//         cout << "我是新线程。。。" << endl;
//         sleep(1);
//     }
// }

// int main()
// {
//     thread t1(thread_run);
//     while(true)
//     {
//         cout << "我是主线程。。。" << endl;
//         sleep(1);
//     }
//     t1.join();

//     return 0;
// }



#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using namespace std;

class ThreadData
{
public:
    int number;
    pthread_t tid;
    char namebuffer[64];
};

class TreadReturn
{
public:
    int number;
    char *name;
};

// 可重入的函数
void *start_routine(void* args)
{
    // sleep(1);
    ThreadData *td = static_cast<ThreadData*>(args); // 安全的强制类型转化
    int cnt = 10;
    while(cnt)
    {
        cout << "cnt:" << cnt << " &cnt:" << &cnt << endl;
        cnt--;
        sleep(1);
        // exit(0);

        // sleep(1);
        // cout << "new thread create success, name:" << td->namebuffer << " cnt:" << cnt-- << endl;
        // sleep(1);
        // int *p = nullptr;
        // *p = 0;
    }
    // delete td;
    // td = nullptr;
    // pthread_exit(nullptr);
    // return nullptr;
    // return (void*)td->number; // warning, void *ret = (void*)td->number;
    // pthread_exit((void*)111);
    // TreadReturn* tr = new TreadReturn;
    // char c[64] = "sada";
    // tr->name = "shuaishuai";
    // tr->number = 20;
    return (void*)10;
}

int main()
{
    // 1.想创建一批接口
    vector<ThreadData*> threads;
#define NUM 10
    for(int i = 0; i < NUM; i++)
    {
        // pthread_t tid;
        ThreadData *td = new ThreadData();
        td->number = i + 1;
        // char namebuffer[64];
        snprintf(td->namebuffer, sizeof(td->namebuffer), "%s:%d", "thread", i);
        // pthread_create(&tid, nullptr, start_routine, (void*)"thread new");
        pthread_create(&td->tid, nullptr, start_routine, td);
        threads.push_back(td);
        
    }

    sleep(5);
    for(int i = 0; i < threads.size()/2; i++)
    {
        pthread_cancel(threads[i]->tid);
        cout << "cancel thread is " << threads[i]->tid << endl;
    }

    // for(auto &iter : threads)
    // {
    //     cout << "creat thread: " << iter->namebuffer << ":" << iter->tid << "  success" << endl;
    // }

    for(auto &iter:threads)
    {
        void *ret = nullptr;
        int n = pthread_join(iter->tid, &ret); // void ** retp = return (void*)td->tid;
        assert(n == 0);
        // (TreadReturn*)ret;
        cout << "join: " << iter->namebuffer << "success, number:" << (long long)ret << endl;
        delete iter;
        iter = nullptr;
    }

    cout << "main thread quit" << endl;

    // pthread_t tid;
    // pthread_create(&tid, nullptr, start_routine, (void*)"thread new");
    
    // while(true)
    // {
    //     cout << "new thread create success, name:" << "main thread" << endl;
    //     sleep(1);
    // }
    
    return 0;
}