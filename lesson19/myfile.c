#include <stdio.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>

int main()
{
    int n = open("shuaishai", O_WRONLY || O_W);
    return 0;
}
