# include "calServer.hpp"
# include "protocol.hpp"
#include <memory>

using namespace server;
using namespace std;

static void Usage(string proc)
{
    cout << "Usage:\n\t" << proc << " local_ip local_port\n\n";
}

// req一定是我们的处理好的完整的请求对象
// resp：根据req进行业务处理，填充resp，不用管任何读取和写入，序列化和反序列化等任何细节
bool cal(const Request& req, Response& resp)
{
    // req已经有结构化完成的数据了 可以直接使用
    resp._exitcode = 0;
    resp._result = OK;
    switch(req._op)
    {
    case '+':
        resp._result = req._x + req._y;
        break;
    case '-':
        resp._result = req._x - req._y;
        break;
    case '*':
        resp._result = req._x * req._y;
        break;
    case '/':
    {   
        if(req._y == 0) resp._exitcode = DIV_ZERO; // 
        else resp._result = req._x / req._y;
    }
        break;
    case '%':
    {   
        if(req._y == 0) resp._exitcode = MOD_ZERO; // 
        else resp._result = req._x % req._y;
    }
        break;
    default:
        resp._exitcode = OP_ERROR;
        break;
    }
    return true;
}

// tcp服务器，在启动上与之前的udp server一模一样
// ./tcpServer localport
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    
    unique_ptr<CalServer> tsvr(new CalServer(port));

    tsvr->initServer();
    tsvr->start(cal);

    return 0;
}