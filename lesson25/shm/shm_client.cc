#include "common.hpp"
using namespace std;

int main()
{
    key_t k = getKey();
    printf("0x%x\n", k);

    int shmId = getShm(k);
    printf("shmId:%d\n", shmId);
    
    // 关联
    char *start = (char*)attachShm(shmId);
    printf("sttach success, address start: %p\n", start);
    // 使用
    const char* message = "hello server, 我是另一个进程，正在和你通信！";
    pid_t id = getpid();
    int cnt = 1;
    // char buffer[1024];
    while(true)
    {
        sleep(1);
        // 直接将需要传递的信息写在共享内存字符串中 省去了很多拷贝的过程 提高了传输信息的效率
        snprintf(start, MAX_SIZE, "%s[pid:%d][消息编号:%d]", message, id, cnt++);
        // snprintf(buffer, sizeof(buffer), "%s[pid:%d][消息编号:%d]", message, id, cnt);
        // memcpy(start, buffer, strlen(buffer)+1);
    }

    // 去关联
    detachShm(start);
    // done

    return 0;
}