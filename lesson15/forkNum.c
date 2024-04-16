#include<stdio.h>
#include<unistd.h>

int main()
{
    int cnt = 0;
    while(1)
    {
        int ret = fork();
        if(ret<0)
        {
            printf("fork error!,cnt=%d\n", cnt);
            break;
        }
        // child进程
        else if(ret==0)
        {
            while(1) sleep(1);
        }
        cnt++;
        // parent
    }
    return 0;
}
