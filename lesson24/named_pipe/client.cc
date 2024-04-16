#include <iostream>
#include "comm.hpp"
using namespace std;

int main()
{
    // 与server打开同一个文件
    cout << "client begin" << endl;
    
    int wfd = open(NAMED_PIPE, O_WRONLY);
    cout << "client end" << endl;    
    if(wfd < 0) 
    {
        cout << "文件打开失败！" << endl;
        exit(1);
    }

    // write
    char buffer[1024];
    while(true)
    {
        cout << "Please Say# ";
        fgets(buffer, sizeof(buffer)-1, stdin);
        if(strlen(buffer) > 0) buffer[strlen(buffer)-1] = 0;
        ssize_t s = write(wfd, buffer, strlen(buffer));
        assert(s == strlen(buffer));
        (void)s;
    }
    

    close(wfd);

    return 0;
}
