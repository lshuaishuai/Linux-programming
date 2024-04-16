#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <cassert>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define PROCESS_NUM 5
#define MakeSeed() srand((unsigned long)time(nullptr) ^ getpid() ^ 0x342 ^ rand()%2343)

typedef void(*func_t)(); // 函数指针

class subEp
{
public:
    subEp(pid_t subId, int writeFd)
        :_writeFd(writeFd)
        ,_subId(subId)
    {
        char nameBuffer[1024];
        snprintf(nameBuffer, sizeof nameBuffer, "process-%d[pid(%d)-fd(%d)]", _num++, _subId, _writeFd);
        _name = nameBuffer;
    }
public:
    static int _num;
    int _writeFd;
    pid_t _subId;
    string _name;
};
int subEp::_num = 0;

void sendTask(const subEp &sub, int taskIdx);
int receiveTask(int readFd);

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
    // assert(out!=nullptr);
    out->push_back(downLoadTask);
    out->push_back(ioTask);
    out->push_back(flushTask);
}

int receiveTask(int readFd)
{
    int code;
    ssize_t s = read(readFd, &code, sizeof(code));
    if(s == 4) return code;
    else if(s <=0 ) return -1;
    else return 0;
}

void creatSubProcess(vector<subEp>* subs, const vector<func_t>& funcMap)
{
    vector<int> deleteFd;
    for(int i = 0; i < PROCESS_NUM; i++)
    {
        int fds[2];
        // 创建成功返回0 失败返回-1
        int n = pipe(fds);
        assert(n == 0);
        (void)n;
        pid_t id = fork();
        if(id == 0)
        {
            for(int i = 0; i < deleteFd.size(); i++) close(deleteFd[i]);
            // 子进程负责读
            close(fds[1]);
            while(true)
            {
                // 读文件 然后进行相关的操作
                int commandCode = receiveTask(fds[0]);
                if(commandCode >= 0 && commandCode < funcMap.size()) funcMap[commandCode]();
                else if(commandCode == -1) break; 
            }
            exit(0);
        }
        // 父进程 写文件
        close(fds[0]);
        subEp sub(id, fds[1]);
        subs->push_back(sub);
        deleteFd.push_back(fds[1]);
    }
}

void sendTask(const subEp &sub, int taskIdx)
{
    cout << "send task num: " << taskIdx << " send to -> " << sub._name << endl;
    ssize_t s = write(sub._writeFd, &taskIdx, sizeof(taskIdx));
    assert(s == sizeof(taskIdx));
    (void)s;
}



void loadBlanceContrl(const vector<subEp>& subs, vector<func_t>& funcMap, int taskCnt)
{
    int processNum = subs.size();
    int taskNum = funcMap.size();
    bool forever = (taskCnt == 0 ? true : false);
    while(true)
    {
        // 选择一个任务
        int taskIdx = rand() % taskNum;
        // 选择一个进程
        int processIdx = rand() % processNum;
        // 将任务吗发送给选择的进程
        sendTask(subs[processIdx], taskIdx);
        sleep(1);

        if(!forever)
        {
            taskCnt--;
            if(taskCnt == 0) break;
        }
    }
    for(int i = 0; i < processNum; i++) close(subs[i]._writeFd);
}

void waitProcess(const vector<subEp>& subs)
{
    int processNum = subs.size();
    for(int i = 0; i < processNum; i++)
    {
        waitpid(subs[i]._subId, nullptr, 0);
        cout << "wait sub process sucess..." << subs[i]._subId << endl;
    }
}

int main()
{
    MakeSeed();
    vector<func_t> funcMap;
    loadTaskFunc(&funcMap);
    // 1.创建子进程和与子进程通信的信道
    vector<subEp> subs;
    creatSubProcess(&subs, funcMap);
    // 2.父进程 控制子进程 负载均衡
    int taskCnt = 3;
    loadBlanceContrl(subs, funcMap, taskCnt);
    // 3.回收子进程


    return 0;
}