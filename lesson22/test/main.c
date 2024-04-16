#include "my_add.h"
#include "my_sub.h"

int main()
{
    int a = 10;
    int b = 20;
    int ret = Sub(a, b);
    printf("result is %d\n", ret);
    ret = Add(a, b);
    printf("result is %d\n", ret);

    return 0;
}
