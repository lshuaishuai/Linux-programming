#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

int main()
{
    pid_t id = fork();
    
    if(id==0)
    {
        // 子进程
        while(1)
        {
            printf("子进程！pid：%d，ppid：%d, id:%d\n", getpid(), getppid(), id);
            sleep(1);
        }
    }
    else if(id>0)
    {
        // parent
        while(1)
        {
            printf("父进程！pid：%d，ppid：%d, id:%d\n", getpid(), getppid(), id);
            sleep(1);
        }
    }
    else{

    }
    //printf("我是一个进程，pid:%d, ppid:%d, id:%d\n", getpid(), getppid(), id);
    // sleep(2);
    //while(1)
   // {
   //     printf("帅帅！我的子进程id是：%d，父进程pid：%d\n",getpid(), getppid());
    //    sleep(1);
   //usr }


    return 0;
}
