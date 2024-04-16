#include "common.hpp"
using namespace std;

int main()
{
    key_t k = getKey();
    printf("0x%x\n", k);

    // 申请共享内存
    int shmId = creatShm(k);
    printf("shmId:%d\n", shmId);
    sleep(3);
    // 关联
    // 将共享内存看为一个字符串
    char *start = (char*)attachShm(shmId);
    printf("sttach success, address start: %p\n", start);
    // 使用
    while(true)
    {
        printf("client say: %s\n", start);
        sleep(1);
    }
    // 去关联
    detachShm(start);
    sleep(5);
    // 删除共享内存
    delShm(shmId);
    return 0;
}