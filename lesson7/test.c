#include<stdio.h>

#define M = 1234

int main()
{
    printf("hello1\n");
    printf("hello2\n");
    printf("hello3\n");
    printf("hello4\n");
    // shuaishuai
    // shuaishuai
 
    // 测试条件编译
#ifdef SHOW 
    printf("hello show\n");
else
    printf("hello default\n");

#endif
    // 测试宏
    print("宏：%d\n", M);
    return 0;
}
