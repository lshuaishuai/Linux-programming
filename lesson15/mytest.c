#include<stdio.h>
#include<string.h>

int AddToTarget(int from, int to)
{
    int sum = 0;
    for(int i=from; i < to; i++)
    {
        sum += i;
    }
    return sum;
}

int main()
{
    for(int i=0;i<200;i++)
    {
        printf("%d:%s\n",i,strerror(i));
    }
    // int sum = AddToTarget(1,100);
    // printf("%d",sum);
    // if(sum==5050)
         //  return 0;
    // else 
    //  return 1;
    // 进程退出的时候，对应的退出码
    return 0;
}
