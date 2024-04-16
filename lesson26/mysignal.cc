#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

void handler(int signo)
{
    cout << "进程捕捉到一个信号，信号编号为：" << signo << endl;
    exit(0);
}

int main()
{
    // 这里为signal函数的调用，并不是handler的调用
    // 只是设置了2号信号的捕捉方法
    // 收到对应的信号才会执行
    signal(2, handler);
    while(1)
    {
        cout << "我是一个进程： " << getpid() << endl;
        sleep(1);
    }
    return 0;
}