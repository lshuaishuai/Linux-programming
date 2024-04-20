#include "BlockQueue.hpp"
#include "Task.hpp"
#include <ctime>
#include <sys/types.h>
#include <unistd.h>

int myAdd(int x, int y)
{
    return x + y;
}

void *consumer(void *_bq)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task>*>(_bq);
    while(true)
    {
        // 消费活动
        int data;
        Task t;
        bq->pop(&t);
        std::cout << "消费数据：" << t() << std::endl;
        sleep(1);
    }
    return nullptr;
}

void *productor(void *_bq)
{
    BlockQueue<Task> *bq = static_cast<BlockQueue<Task>*>(_bq);
    while(true)
    {
        // 生产活动
        int x = rand() % 10 + 1; // 1-10 先用随机数构建数据
        int y = rand() % 5 + 1;
        Task t(x, y, myAdd);
        bq->Push(t);
        std::cout << "生产数据：" << "发送" << x << "与" << y << "相加的任务！" <<  std::endl;
    }
    return nullptr;
}

int main()
{
    srand((unsigned long)time(nullptr) ^ getpid());
    BlockQueue<Task> *bq = new BlockQueue<Task>();   
    pthread_t c, p;
    pthread_create(&c, nullptr, consumer, bq);
    pthread_create(&p, nullptr, productor, bq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    return 0;
}