#include "common.hpp"
using namespace std;

void creatReceiverProc(vector<ReceiverEp>& recs, vector<func_t>& funcMap)
{
    bool r = createFifo(NAMED_PIPE);
    assert(r==true);
    (void)r;
    int rfd = open(NAMED_PIPE, O_RDONLY);
    if(rfd < 0)
    {
        cout << "文件打开失败" << endl;
        exit(1);
    }
    int commandCode = recvTask(rfd);
    if(commandCode >=0 && commandCode < funcMap.size()) funcMap[commandCode]();
    else if(commandCode == -1);
}

int main()
{
    vector<func_t> funcMap;
    loadTaskFunc(&funcMap);
    bool r = createFifo(NAMED_PIPE);
    assert(r);
    (void)r;

    int rfd = open(NAMED_PIPE, O_RDONLY);  // 只读的方式打开
    if(rfd < 0)
    {
        cout << "文件打开失败！" << endl;
        exit(1);
    }

    // read
    int code;
    while(true)
    {
        ssize_t s = read(rfd, &code, sizeof(code));
        if(s > 0)   
        {
            cout << "sender -> receiver code: " << code << endl;
            funcMap[code]();
        }
        else if(s == 0)
        {
            cout << "sender quit, me too！" << endl;
            break;
        }
        else{
            cout << "err string: " << strerror(errno) << endl;
            break;
        }
    }
    close(rfd);
    removeFifo(NAMED_PIPE);
    
    return 0;
}