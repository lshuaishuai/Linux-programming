#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<fcntl.h>
#include<assert.h>

#define FILE_NAME "log.txt"

// #define ONE (1<<0)
// #define TWO (1<<1)
// #define THREE (1<<2)
// #define FOUR (1<<3)
 
int main()
{
    int fp = open(FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0666);
    
    assert(fp != -1);
    char buffer[64];
    int cnt = 5;
    while(cnt)
    {
        sprintf(buffer, "%s:%d\n", "ting", cnt--);
        write(fp, buffer, strlen(buffer));
    }
     // write(fp, "1234\n", 11);
    close(fp);
    return 0;
}
