#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void *func(void *args)
{
    while(true)
    {
        cout << "我是新创建的线程：" << args << endl;
        sleep(1);
    }
}

int main()
{
    pthread_t tid;

    pthread_create(&tid, nullptr, func, (void*)"thread_one");

    while(true)
    {
        cout << "我是主线程" << endl;
        sleep(1);
    }

    return 0;
}