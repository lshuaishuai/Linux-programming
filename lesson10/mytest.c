#include<stdio.h>
#include<time.h>
#include<unistd.h>

void Print(int sum)
{
    long long timestamp = time(NULL);
    printf("result=%d, timestamp: %lld\n", sum, timestamp);
}

int AddToval(int from, int to)
{
    int sum=0;
    for(int i=from; i<=to; i++)
    {
        sum = sum+i;;
    }
    return sum;
}

int main()
{   
    int sum=AddToval(0, 100);
    Print(sum);
    sleep(3);
    return 0;
}
