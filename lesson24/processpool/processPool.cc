#include <iostream>
#include <unistd.h>
#include <cassert>
using namespace std;
#include <vector>
#include <string>
#include <ctime>
#include <functional>
#include <sys/wait.h>
#include <sys/types.h>

#define MakeSeed() srand((unsigned long)time(nullptr) ^ getpid() ^ 0x171237 ^ rand()%1234)

#define PROCSS_NUM 10

// /////////////////子进程要完成的的某种任务 -- 模拟一下/////////////
// 函数指针类型
typedef void(*func_t)();

void downLoadTask()
{
    cout << getpid() << ":下载任务" << endl;
    sleep(1);
}

void ioTask()
{
    cout << getpid() << ":IO任务" << endl;
    sleep(1);
}

void flushTask()
{
    cout << getpid() << ":刷新任务" << endl;
    sleep(1);
}

void loadTaskFunc(vector<func_t> *out)
{
    assert(out!=nullptr);
    out->push_back(downLoadTask);
    out->push_back(ioTask);
    out->push_back(flushTask);
}

////////////////////////////下面的代码为一个多进程程序
class SubEp // endpoint
{
public:
    SubEp( pid_t subId, int writeFd)
        :_subId(subId)
        ,_writeFd(writeFd)
    {
        char nameBuffer[1024];
        snprintf(nameBuffer, sizeof nameBuffer, "process-%d[pid(%d)-fd(%d)]", _num++, _subId, _writeFd);
        _name = nameBuffer;
    }
public:
    static int _num;
    string _name;
    pid_t _subId;
    int _writeFd;
};
// 静态变量就，类内声明 类外初始化
int SubEp::_num = 0;

int recvTask(int readFd)
{
    int code;
    // 返回实际读取的字节数  在读取过程中出现错误，它会返回一个负值，表示相应的错误码
    ssize_t s = read(readFd, &code, sizeof(code));
    if(s == 4) return code;
    else if(s <= 0) return -1;
    else return 0;
}

void sendTask(const SubEp &process, int taskNum)
{
    cout << "send task num: " << taskNum << " send to -> " << process._name << endl;
    // 返回实际写入的字节数。如果在写入过程中出现错误，它会返回一个负值，表示相应的错误码。
    int n = write(process._writeFd, &taskNum, sizeof(taskNum));
    assert(n == sizeof(int));
    (void)n;
}

void creatSubProcess(vector<SubEp> *subs, vector<func_t> &funcMap)
{
    vector<int> deleteFd;
    for(int i = 0; i < PROCSS_NUM; i++)
    {
        int fds[2];
        // 创建成功则返回0 失败返回-1
        int n = pipe(fds);
        assert(n == 0);
        (void)n;
        // 父进程打开的文件是会被子进程共享的
        pid_t id = fork();
        if(id == 0)
        {
            
            for(int i=0; i < deleteFd.size(); i++) close(deleteFd[i]);
            // 子进程 进行处理任务
            close(fds[1]);
            while(1)
            {
                // 1.获取命令码，如果没有发送，我们子进程应该阻塞
                int commandCode = recvTask(fds[0]);
                // 2.完成任务
                if(commandCode >= 0 && commandCode < funcMap.size()) funcMap[commandCode]();
                else if(commandCode == -1) break;
            }
            exit(0);
        }

        close(fds[0]);
        SubEp sub(id, fds[1]);
        subs->push_back(sub);
        deleteFd.push_back(fds[1]);
    }
}

void loadBlanceContrl(const vector<SubEp>& subs, vector<func_t>& funcMap, int taskCnt)
{
    int processnum = subs.size();
    int tasknum = funcMap.size();
    bool forever = (taskCnt == 0 ? true : false);
    while(true)
    {
        // 1.先选择一个子进程 --> vector<SubEp> -> index
        // 将我们的任务均衡的下发给每一个子进程，让子进程负载均衡 -- 单机版  随机数法
        int subIdx = rand() % processnum;

        // 2.选择一个任务 --> vector<func_t> -> index
        int taskIdx = rand() % tasknum;

        // 3.任务发送给选择的进程
        sendTask(subs[subIdx], taskIdx);
        sleep(1);
        if(!forever)
        {
            taskCnt--;
            if(taskCnt == 0) break;
        }
    }

    // write quite -> read 0
    for(int i = 0; i < processnum; i++) close(subs[i]._writeFd);
}

void waitProcess(const vector<SubEp>& processes)
{
    int processNum = processes.size();
    for(int i = 0; i < processNum; i++)
    {
        waitpid(processes[i]._subId, nullptr, 0);
        cout << "wait sub process sucess..." << processes[i]._subId << endl;
     }
}

int main()
{
    MakeSeed();
    // 1.建立子进程并建立和子进程通信的信道 目前是有BUG的
    // [子进程id，文件描述符]
    // 1.1 加载方法表
    vector<func_t> funcMap; // 函数指针表
    loadTaskFunc(&funcMap);
    // 1.2 创建子进程 并且维护好父子通信信道
    vector<SubEp> subs; 
    creatSubProcess(&subs, funcMap);

    // 2.走到这里的就是父进程 控制子进程 负载均衡的像子进程发送命令码
    int taskCnt = 3;  // 0为永远进行
    loadBlanceContrl(subs, funcMap, taskCnt);
    // 3.回收子进程
    waitProcess(subs);
    
    return 0;
}