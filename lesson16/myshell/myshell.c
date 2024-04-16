#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<assert.h>
#include<string.h>

#define NUM 1024    // 一行做多1024个字符
#define OPT_NUM 64  // 最多64个选项
char lineCommand[NUM];
char *myargv[OPT_NUM];
int lastCode = 0;
int lastSig = 0;

int main()
{
    while(1)
    {

        // 打印输出提示符
        printf("用户名@主机名 当前路径# ");
        // fflush(stdout);
    
        // 获取用户输入 输入的时候 输入\n
        char *s = fgets(lineCommand, sizeof(lineCommand)-1, stdin);

        assert(s != NULL);
    
        (void)s;
        // 清除最后一个\n
        lineCommand[strlen(lineCommand)-1] = 0; // ?
        // printf("test: %s\n", lineCommand);
        // "ls -a -l -i" -> "ls" "-a" "-i" "NULL"
        // 字符串切割
        int i = 1;
        myargv[0] = strtok(lineCommand, " ");
        if(myargv[0] != NULL && strcmp(myargv[0], "ls") == 0)
        {
            myargv[i++] = (char*)"--color=auto";
        }
        
        while(myargv[i++]=strtok(NULL, " "));
        // 如果是cd命令，不需要创建子进程，让shell自己执行对应的命令，本质是执行系统接口
        // 像这种不需要子进程执行的程序，而是让shell自己执行的命令，叫做内置/内建命令
        if(myargv[0] != NULL && strcmp(myargv[0], "cd") == 0)
        {
            if(myargv[1] != NULL) chdir(myargv[1]);
            continue;
        }

        if(myargv[0] != NULL && myargv[1] != NULL && strcmp(myargv[0], "echo") == 0 && strcmp(myargv[1], "$?") == 0)
        {
            if(strcmp(myargv[1], "$?") == 0)
            {
                printf("%d, %d\n", lastCode, lastSig);
            }
            else{
                printf("%s\n", myargv[1]);
            }
            continue;
        }

#ifdef DEBUG
        // 测试是否成功
        for(int i=0;myargv[i];i++)
        {
            printf("myargv[%d]:%s\n", i, myargv[i]);
        }
#endif
        pid_t id = fork();
        assert(id!=-1);

        if(id == 0)
        {
            // 子进程
            // 让子进程进行操作
            execvp(myargv[0], myargv);
            exit(1);
        }
        int status = 0;
        
        pid_t ret = waitpid(id, &status, 0);
        assert(ret>0);
        (void)ret;
        lastCode = ((status>>8) & 0xFF);
        lastSig = (status & 0x7F);
        // printf("shuaishuai");
    }
    return 0;
}
