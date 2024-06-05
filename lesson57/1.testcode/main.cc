# include "util.hpp"
# include <cstdio>
# include <vector>
# include <functional>

using func_t = std::function<void ()>;

#define INIT(v) do{\
    v.push_back(printLog);\
    v.push_back(download);\
    v.push_back(executeSql);\
    }while(0)

#define EXEC_OTHER(cbs) do{\
    for(const auto& cb : cbs) cb();\
    }while(0)

int main()
{
    std::vector<func_t> cbs;
    INIT(cbs);

    SetNonBlock(0);
    char buffer[1024];
    while(true)
    {
        // printf(">> ");
        // fflush(stdout);
        ssize_t s = read(0, buffer, sizeof(buffer)-1);
        if(s > 0)
        {
            buffer[s] = 0;
            std::cout << "echo# " << buffer << std::endl;
        }
        else if(s == 0)
        {
            std::cout << "read end" << std::endl;
            break;
        }
        else{
            // std::cout << "......" << std::endl;
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                std::cout << "我没错，是底层暂无数据！" << std::endl;
                EXEC_OTHER(cbs);
            }
            else if(errno == EINTR) // 被信号中断
            {
                continue;
            }
            else{
                std::cout << "result: " << s << " errno: " << errno << " " << strerror(errno) << std::endl;
                break;
            }
        }
        sleep(1);
    }

    return 0;
}