#include <iostream>
#include <signal.h>
#include <cstdio>
#include <unistd.h>

using namespace std;

void Count(int cnt)
{
    while(cnt)
    {
        printf("cnt: %2d\r", cnt--);
        fflush(stdout);
        sleep(1);
    }
    cout << endl;
}

void handler(int signo)
{
    cout << "正在处理" << signo << "号信号" << endl;
    Count(20);
}

int main()
{
    struct sigaction act, oact;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    // sigaddset(&act.sa_mask, 3);

    sigaction(SIGINT, &act, &oact);
    sigaction(3, &act, &oact);
    while(1)
    {
        cout << "我是一个进程" << endl;
        sleep(1);
    }

    return 0;
}