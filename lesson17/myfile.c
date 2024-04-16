#include<stdio.h>
#include<unistd.h>
#include<string.h>


#define FILE_NAME "log.txt"

int main()
{
   //  FILE *fp = fopen(FILE_NAME, "w");  // r:读  w:写  r+：读写，不存在就出错  w+：读写，不存在就创建,a（append，追加）， a+（）
    
   // FILE *fp = fopen(FILE_NAME, "r");  // r:读  w:写  r+：读写，不存在就出错  w+：读写，不存在就创建,a（append，追加）， a+（）
    
    FILE *fp = fopen(FILE_NAME, "a");  // r:读  w:写  r+：读写，不存在就出错  w+：读写，不存在就创建,a（append，追加）， a+（）
    if(fp == NULL)
    {
        perror("fopen"); // ? 
        return 1;
    }

   // char buffer[64];
   // while(fgets(buffer, sizeof(buffer)-1, fp)!=NULL)
   // {
   //     buffer[strlen(buffer)-1] = 0;
   //     puts(buffer);
   // }

    int cnt = 5;
    while(cnt)
    {
        fprintf(fp, "%s:%d\n", "hello shuaishuai", cnt--); // 文件写入
    }

    fclose(fp);

//    while(1)
//    {
//        printf("我是一个进程：%d\n", getpid());
//    }
//
//    return 0;
}
