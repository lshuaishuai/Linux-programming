# include <iostream>
# include <string>
# include <unistd.h> 
# include <pthread.h>

int tickets = 1000;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *start_routine(void *args)
{
    std::string name = static_cast<const char*>(args);
    while(true)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex); // 为什么要有mutex
        // 判断暂时省略
        std::cout << name << "->" << tickets << std::endl;
        tickets--;
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{   
    // 通过条件变量控制线程的执行
    pthread_t t1, t2, t3, t4, t5;
    pthread_create(&t1, nullptr, start_routine, (void*)"thread 1");
    pthread_create(&t2, nullptr, start_routine, (void*)"thread 2");
    pthread_create(&t3, nullptr, start_routine, (void*)"thread 3");
    pthread_create(&t4, nullptr, start_routine, (void*)"thread 4");
    pthread_create(&t5, nullptr, start_routine, (void*)"thread 5");


    while(true)
    {
        sleep(1);
        pthread_cond_signal(&cond);
        // pthread_cond_broadcast(&cond);
        
        std::cout << "main thread wakeup one thread..." << std::endl;
    }

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);
    pthread_join(t4, nullptr);
    pthread_join(t5, nullptr);

    return 0;
}