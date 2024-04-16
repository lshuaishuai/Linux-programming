#include "common.hpp"
using namespace std;

int main()
{   
    MakeSeed();
    umask(0);
    int wfd = open(NAMED_PIPE, O_WRONLY, 0600);
    if(wfd < 0)
    {
        cout << "文件打开失败" << endl;
        exit(1);
    }
    
    int code;
    while(true)
    {
        cout << "please Say# ";
        int taskIdx = rand() % 3;
        cout << "taskCode:" << taskIdx << endl;
        ssize_t s = write(wfd, &taskIdx, sizeof(code));
        assert(s == sizeof(taskIdx));
        (void)s;
        sleep(1);
    }

    close(wfd);

    return 0;
}
