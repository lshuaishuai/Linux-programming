#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <vector>

#define MAXSIGNUM 31

using namespace std;

static vector<int> sigarr = {2};

void printPending(sigset_t *pending)
{
    for(int i = MAXSIGNUM; i >= 1; i--)
    {
        if(sigismember(pending, i)) cout << "1";
        else cout << "0";
    }
    cout << endl;
}

int main()
{
    // 1.先尝试屏蔽指定的信号
    sigset_t block, oblock, pending;
    // 1.1 初始化
    sigemptyset(&block);  // 将位图结构中都置为0
    sigemptyset(&oblock);  // 将位图结构中都置为0
    sigemptyset(&pending);  // 将位图结构中都置为0
    // 1.2 添加要屏蔽的信号
    //for(const auto &e:sigarr) 
    sigaddset(&block, 2);
    // 1.3 开始屏蔽
    sigprocmask(SIG_SETMASK, &block, &oblock);

    // 2.遍历打印pending的信号集
    int cnt = 10;
    while(1)
    {
        // 2.1 初始化pending信号集
        sigisemptyset(&pending);
        // 2.2 获取当前进程的未决信号集
        sigpending(&pending);
        // 2.3 打印
        printPending(&pending);

        sleep(1);
        if(cnt-- <= 0)
        {
            cout << "恢复对信号的屏蔽，不屏蔽任何信号\n" << endl;
            sigprocmask(SIG_SETMASK, &oblock, &block);
        }
    }


    return 0;
}