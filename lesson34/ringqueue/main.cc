# include "RingQueue.hpp"
# include "Task.hpp"
# include <pthread.h> 
# include <unistd.h>
# include <ctime>
# include <cstdlib>
# include <sys/types.h>

using namespace std;

string SelfName()
{
    char buffer[128];
    snprintf(buffer, sizeof buffer, "0x%x", pthread_self());
    return buffer;
}

void *ProductorRoutine(void *args)
{
    // RingQueue<int> *rq = static_cast<RingQueue<int> *>(args);
    RingQueue<Task> *rq = static_cast<RingQueue<Task> *>(args);
    while(true)
    {
        // Version1
        // sleep(2);
        // int data = rand() % 10 + 1;
        // rq->Push(data);
        // cout << "生产完成，生产的数据是：" << data << endl;
    
        // Version2
        // 模拟构建一个任务
        int x = rand() % 1000;
        int y = rand() % 1500;
        char op = oper[rand() % oper.size()];
        Task t(x, y, op, myMath);
        // 生产任务
        rq->Push(t);
        // 输出提示
        cout << SelfName() << "生产者排发了一个任务：" << t.toTaskString() << " " << endl;
        
    }
}

void *ConsumerRoutine(void *args)
{
    // RingQueue<int> *rq = static_cast<RingQueue<int> *>(args);
    RingQueue<Task> *rq = static_cast<RingQueue<Task> *>(args);
    
    while(true)
    {
        // verson1
        // int data;
        // rq->Pop(&data);
        // cout << "消费完成，消费的数据是：" << data << endl;
        
        // Version2
        sleep(2);
        Task t;
        // 消费任务
        rq->Pop(&t);
        string ret = t();
        cout << SelfName() <<"消费者消费了一个任务：" << ret << " " << endl;
    }
}


int main()
{
    srand((unsigned int)time(nullptr) ^ getpid() ^ pthread_self() ^ 0x71727374);
    // RingQueue<int> * rq = new RingQueue<int>();
    RingQueue<Task> * rq = new RingQueue<Task>();

    // 单生产 单消费 多生产 多消费 --》 只要保证，最终进入临界区的是一个生产者一个消费者就好了
    // 多生产多消费意义？ 与前面一样 获取构建任务/完成任务 是需要时间的 
    // pthread_t c[8], p[4];
    // for(int i = 0; i < 4; i++) pthread_create(p+i, nullptr, ProductorRoutine, rq);
    // for(int i = 0; i < 8; i++) pthread_create(c+i, nullptr, ConsumerRoutine, rq);

    // for(int i = 0; i < 4; i++) pthread_join(p[i], nullptr);
    // for(int i = 0; i < 8; i++) pthread_join(c[i], nullptr);

    pthread_t c, p; // c:消费者 p:生产者

    pthread_create(&p, nullptr, ProductorRoutine, rq);
    pthread_create(&c, nullptr, ConsumerRoutine, rq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    delete rq;
    return 0;
}