#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>

int main()
{
    pid_t id = fork();
    if(id == 0)
    {
        int cnt = 5;
        while(cnt)
        {
            printf("我是子进程pid：%d, 父进程：%d, cnt = %d\n", getpid(), getppid(), cnt--);
            sleep(1);
        }
        
        // int d = 10/0;
        exit(10);
    }
    sleep(7);
    int status = 0;
    pid_t ret = waitpid(id, &status, 0);
    if(id > 0)
    {
        printf("wait success: %d, sig_number: %d, child_exit_code: %d\n", ret, (status & 0x7F), (status>>8)&0xFF);
    }
    sleep(5);

    return 0;
}
