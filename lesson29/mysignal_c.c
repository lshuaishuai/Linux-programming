#include <stdio.h>
#include <signal.h>

volatile quit = 0;

void handle(int signo)
{
    printf("%d号信号，正在被捕捉！\n", signo);
    printf("quit: %d", quit);
    quit = 1;
    printf(" -> quit: %d", quit);
}

int main()
{
    signal(2, handle);
    while (!quit)
    { 
        printf("正在循环！\n");
        sleep(1); 
    };
    printf("注意，我是正常退出的！\n");
    
    return 0;
}