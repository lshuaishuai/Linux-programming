#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cassert>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
using namespace std;

int main()
{
    // 第一步：创建管道文件
    int fds[2];
    int n = pipe(fds);
    assert(n == 0);
    // 0 1 2应该是被占用的 _-> 3 4
    cout << "fds[0]: " << fds[0] << endl;
    cout << "fds[1]: " << fds[1] << endl;
    // 第二步：fork
    pid_t id = fork();
    assert(id >= 0);
    if(id == 0)
    {
        // 子进程的通信代码 子进程写入
        close(fds[0]);
        // 通信代码
        // string msg = "hello, i am child!";
        int cnt = 0;
        const char* s = "我是子进程，我正在给你发消息！";
        while(1)
        {
            cnt++;
            char buffer[1024]; // 只有子进程能看到
            snprintf(buffer, sizeof buffer, "child->parent say: %s[%d][%d]", s, cnt, getpid());
            // 往文件中写入数据
            write(fds[1], buffer, strlen(buffer));
            // sleep(50); // 细节 每隔一秒写一次
            // break;
        }
        
        cout << "子进程关闭写端" << endl;
        close(fds[1]);
        exit(0);
    }
    // 父进程的通信代码  父进程读取
    close(fds[1]);
    while(1)
    {
        char buffer[1024];
        // cout << "AAAAAAAAAAAAAAA" <<endl;
        // 父进程在这里阻塞等待
        ssize_t s = read(fds[0], buffer, sizeof(buffer) - 1);
       //  cout << "BBBBBBBBBBBBBBB" <<endl;
        if(s > 0) 
        {
            buffer[s] = 0;
            cout << " Get Message# " << buffer <<" | my pid: " << getpid() << endl;
        }
        else if(s == 0)
        {
            cout << "read: " << s << endl;
            break;
        }
        // cout << "Get Message#" << buffer << " | my pid: " << getppid() << endl;
        // 细节：父进程可没有进行sleep
        //sleep(5);
        // close(fds[0]);
        break;
    }
    close(fds[0]);
    int status = 0;
    cout << "父进程关闭读端" << endl;
    n = waitpid(id, &status, 0);
    assert(n == id);

    cout << "pid->" << n << ":" << (status & 0x7F) << endl; // 信号为14SIGPIPE 中止了写入进程
    
    
    
    return 0;
}