#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
    int a=0;
    // while(1)
   //  {
   //      a=a+1;
        // printf("当前a的值为：%d\n", a);
    // }
    pid_t id = fork();
    if(id<0)
    {
        perror("fork");
    }
    else if(id == 0)
    {
        // child
        while(1)
        {
            printf("我是子进程，pid:%d, ppid:%d\n",getpid(), getppid());
            sleep(1);
        }
    }
    else 
    {
        // parent
        while(1)
        {
            printf("我是父进程，pid:%d, ppid:%d\n",getpid(), getppid());
            sleep(3);
        }
    }
    return 0;
}
