#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    // FILE* fp = fopen("log.txt", "w");
    // FILE* fp = fopen("log.txt", "r");  // 以读的方式打开
    FILE* fp = fopen("log.txt", "a");  // 以读的方式打开
    if(fp == NULL)
    {
        printf("fopen error!");
        return 1;
    }
    char buffer[64];
   //  while(fgets(buffer, sizeof(buffer)-1, fp) != NULL)
   //  {
   //      buffer[strlen(buffer)-1] = 0;
   //      puts(buffer);
   //  }
    
    int cnt = 5;
    while(cnt)
    {
        fprintf(fp, "%s:%d\n", "shuaishuai", cnt--);
    }

    fclose(fp);


    return 0;
}
