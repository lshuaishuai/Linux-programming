# include "Task.hpp"
# include "ThreadPool.hpp"
# include <memory>

int main()
{
    std::unique_ptr<ThreadPool<Task>> tp(new ThreadPool<Task>());
    // ThreadPool<Task>::getInstance()->run(); // 懒汉模式 只有第一用才会加载 延迟加载
    tp->run();

    int x, y;
    char op;
    while(1)
    {
        std::cout << "请输入数据1# ";
        std::cin >> x;
        std::cout << "请输入数据2# ";
        std::cin >> y;
        std::cout << "请输入将进行的运算#  ";
        std::cin >> op;

        Task t(x, y, op, myMath);
        std::cout << "刚刚录入的任务：" << t.toTaskString() << "，确认提交吗？[y/n]# " << std::endl;
        char confirm;
        std::cin >> confirm;
        if(confirm == 'y') 
        tp->Push(t);
        // ThreadPool<Task>::getInstance()->Push(t);
        
        sleep(1);
    }
    return 0;
}