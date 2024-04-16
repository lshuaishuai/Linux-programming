#include <iostream>
#include "comm.hpp"
using namespace std;

int main()
{
    bool r = createFifo(NAMED_PIPE);
    assert(r);
    (void)r;

    cout << "server begin" << endl;
    int rfd  =open(NAMED_PIPE, O_RDONLY);  // 只读方式打开
    cout << "server end" << endl;

    if(rfd < 0) 
    {
        cout << "文件打开失败！" << endl;
        exit(1);
    }

    // read
    char buffer[1024];
    while(true)
    {
        ssize_t s = read(rfd, buffer, sizeof buffer - 1);
        if(s > 0)
        {
            buffer[s] = 0;
            std::cout << "client->server:" << buffer << endl;
        }
        else if(s == 0)
        {
            cout << "client quit, me too!" << endl;
            break;
        }
        else{
            cout << "err string:" << strerror(errno) << endl;
            break;
        }
    }

    close(rfd);


    // sleep(10);
    removeFifo(NAMED_PIPE);
    return 0;
}
