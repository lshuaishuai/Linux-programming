#include<stdio.h>
#include<unistd.h>

int main()
{
    int cnt=10;
    while(cnt)
    {
        printf("%2d\r",cnt);// 一次刷新两个字符
        fflush(stdout);
        cnt--;
        sleep(1);
    }
    // 一定先执行printf
   //  printf("youcan see me............\n");// \n为行缓冲
    // fflush(stdout); // 直接显示
    // printf -- done
    // 只不过，该数据没有被立即显示出来
    // sleep(2);

    // 两秒后 才刷新的数据
    // 在sleep(2)期间 printf一定执行打印了，那么对应的输出数据先存在了缓冲区中
    
    return 0;
}
