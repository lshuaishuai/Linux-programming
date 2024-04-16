#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>

using namespace std;

static void Usage(const string& proc)
{
    cout << "\nUsage:" << proc << " pid sino\n" << endl;
}

int cnt = 0;

void catchSignal(int signo)
{
    cout << "获得一个信号，信号编号为：" << cnt << endl;
    // exit(1);
}

int main(int argc, char *argv[])
{
    // if(argc != 3)
    // {
    //     Usage(argv[0]);
    //     exit(1);
    // }
    // // 1.通过键盘发送信号
    // // 2.通过系统调用发送信号
    // pid_t pid = atoi(argv[1]);
    // int signo = atoi(argv[2]);
    // int n = kill(pid, signo);
    // if(n != 0)
    // {
    //     perror("kill");
    // }
    // int cnt = 0;
    // while(cnt <= 10)
    // {
    //     cout << "signal ss" << cnt++ << endl;
    //     if(cnt >= 5) raise(9);
    // }
    // while(1)
    // {
    //     sleep(1);
    //     cout << "shuaishuai" << endl;
    // }
    // signal(SIGFPE, catchSignal);
    // while(true)
    // {
    //     cout << "我在运行中...." << endl;
    //     sleep(1);
    //     int a = 10;
    //     a /= 0;
    // }
    // signal(SIGSEGV, catchSignal);
    // while(1)
    // {
    //     int *p = nullptr;
    //     *p = 10;
    // }

    // 4.软件条件

    // 统计1S，计算机能累加多少次  IO很慢
    // signal(SIGALRM, catchSignal);
    // alarm(1);
    // while(1)
    // {
    //     cnt++;
    //     // cout << "cnt = " << cnt++ << endl; // 外设打印 拖慢了计算的节奏
    // }

    // 核心转储问题
    // while(1)
    // {
    //     int a[10];
    //     // a[10000] = 106;
    // }

    // 
    for(int signo = 1; signo < 32l; signo++)
    {
        signal(signo, catchSignal);
    }

    while(1)
    {
        cout << "我是一个进程：" << getpid() << endl;
        sleep(1);
    }
    return 0;
}