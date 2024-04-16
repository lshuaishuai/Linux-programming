#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
    // close(0); // 关闭系统默认打开的文件stdin
    // close(2); 

    umask(0);
    // int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    // int fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0666); // 追加重定向
    // if(fd < 0)
    // {
    //     perror("open");
    //     return 1;
    // }
    // if(dup2(fd, 1)>0)  //将fd拷贝到1
    // {
    //     close(fd);
    // }
    // 
    // printf("open fd:%d\n", fd);
    // fprintf(stdout, "open fd:%d\n", fd);
    // const char* buff = "shuaishuai";
    // write(1, buff, strlen(buff));


    // close(fd);   
    
    int fd = open("log.txt", O_RDONLY);
    if(fd < 0)
    {
        perror("open");
        return 1;
    }
    dup2(fd, 0); // 输入重定位
    char str[64];
    while(1)
    {
        printf("> ");
        if(fgets(str, sizeof(str), stdin) == NULL) break;
        printf("%s", str);
    }

    close(fd);

    return 0;
}
