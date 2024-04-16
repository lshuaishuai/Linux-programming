#include <iostream>
#include <unistd.h>
#include <cassert>
using namespace std;

#define PROCSS_NUM 5

int main()
{
    // 1.建立子进程并建立和子进程通信的信道 目前是由BUG的
    // [子进程id，文件描述符]
    for(int i = 0; i < PROCSS_NUM; i++)
    {
        int fds[2];
        int n = pipe(fds);
        assert(n == 0);
        (void)n;
        pid_t id = fork();
        if(id == 0)
        {
            // 子进程 进行处理任务
            close(fds[1]);
            cout << "shuaishuai" << endl;
            exit(0);
        }

        close(fds[0]);
    }

    // 2.走到这里的就是父进程 控制子进程


    // 3.回收子进程


    return 0;
}