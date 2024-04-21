#include "BlockQueue.hpp"
#include "Task.hpp"
#include <ctime>
#include <sys/types.h>
#include <unistd.h>

// C:计算 S:存储
template <class C, class S>
class BlockQueues
{
public:
    BlockQueue<C> *c_bp;
    BlockQueue<S> *s_bp;
};

// 即使生产者 也是消费者
void *consumer(void *_bqs)
{
    BlockQueue<CalTask> *bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(_bqs)->c_bp);
    BlockQueue<SaveTask> *save_bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(_bqs)->s_bp);

    while(true)
    {
        // 计算活动
        CalTask t;
        bq->pop(&t);
        std::string result = t();
        std::cout << "cal thread，完成计算任务：" << result << "...done" << std::endl;

        // // 推送任务
        // SaveTask save(result, Save);
        // save_bq->Push(save);
        // std::cout << "cal thread，推送存储任务完成..." << std::endl;
        // sleep(1);
    }
    return nullptr;
}

void *productor(void *_bqs)
{
    // BlockQueue<Task> *bq = static_cast<BlockQueue<Task>*>(_bq);
    BlockQueue<CalTask> *bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(_bqs)->c_bp);

    while(true)
    {
        // 生产活动
        sleep(3);

        int x = rand() % 100 + 1; // 1-10 先用随机数构建数据
        int y = rand() % 10;
        int operCode = rand() % oper.size();
        CalTask t(x, y, oper[operCode], myMath);
        bq->Push(t);
        std::cout << "productor thread,生产计算任务：" << t.toTaskString() << std::endl;

    }
    return nullptr;
}

void *saver(void* _bqs)
{
    BlockQueue<SaveTask> *save_bq = (static_cast<BlockQueues<CalTask, SaveTask>*>(_bqs)->s_bp);

    while(true)
    {
        // 新的消费者
        SaveTask t;
        save_bq->pop(&t);
        t();
        std::cout << "save thread，保存任务完成..." << std::endl;

    }
    
}

int main()
{
    // 创建两个块队列 c既是生产者也是消费者(从计算队列读数据并计算并推送记录到保存队列) s是消费者(保存任务) p是生产者(生产计算任务)
    srand((unsigned long)time(nullptr) ^ getpid());
    BlockQueues<CalTask, SaveTask> bqs;

    bqs.c_bp = new BlockQueue<CalTask>();   
    bqs.s_bp = new BlockQueue<SaveTask>(); 
    pthread_t c[2], p[3];
    pthread_create(&p[0], nullptr, productor, &bqs);
    pthread_create(&p[1], nullptr, productor, &bqs);
    pthread_create(&p[2], nullptr, productor, &bqs);

    pthread_create(&c[0], nullptr, consumer, &bqs);
    pthread_create(&c[1], nullptr, consumer, &bqs);

    pthread_t s;
    // pthread_create(&s, nullptr, saver, &bqs);

    pthread_join(c[0], nullptr);
    pthread_join(c[1], nullptr);
    pthread_join(p[0], nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);

    // pthread_join(s, nullptr);

    delete bqs.c_bp;
    delete bqs.s_bp;

    return 0;
}