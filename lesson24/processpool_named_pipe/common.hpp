#pragma once
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cassert>
#include <cerrno>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctime>
using namespace std;

#define MakeSeed() srand((unsigned long)time(nullptr) ^ getpid() ^ 0x171237 ^ rand()%1234)

typedef void(*func_t)();

#define NAMED_PIPE "../mypipe.106"
#define PROCSS_NUM 10

class ReceiverEp // endpoint
{
public:
    ReceiverEp( pid_t subId, int writeFd)
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

bool createFifo(const string& path)
{
    umask(0);
    int n = mkfifo(path.c_str(), 0600);
    if(n == 0) return true;
    else 
    {
        cout << "errno:" << errno << "str err:" << strerror(errno) <<endl;
        return false;
    }
}

void removeFifo(const string& path)
{
    int n = unlink(path.c_str());
    assert(n == 0);
    (void)n;
}

int recvTask(int readFd)
{
    int code;
    // 返回实际读取的字节数  在读取过程中出现错误，它会返回一个负值，表示相应的错误码
    ssize_t s = read(readFd, &code, sizeof(code));
    if(s == 4) return code;
    else if(s <= 0) return -1;
    else return 0;
}