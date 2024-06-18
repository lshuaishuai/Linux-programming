#include "TcpServer.hpp"
#include <memory>

static void usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << " prot" << std::endl;
}

bool cal(const Request &req, Response &resp)
{
    // req已经有结构化完成的数据了 可以直接使用
    resp._exitcode = 0;
    resp._result = OK;
    switch (req._op)
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
        if (req._y == 0)
            resp._exitcode = DIV_ZERO; //
        else
            resp._result = req._x / req._y;
    }
    break;
    case '%':
    {
        if (req._y == 0)
            resp._exitcode = MOD_ZERO; //
        else
            resp._result = req._x % req._y;
    }
    break;
    default:
        resp._exitcode = OP_ERROR;
        break;
    }
    return true;
}

void calculat(tcpserver::Connection *conn)
{
    std::string onePackage;
    while (ParseOnePackage(conn->_inbuffer, &onePackage))
    {
        std::string reqStr;
        if (!deLength(onePackage, &reqStr))
            return;
        std::cout << "去报头的正文： \n"
                  << reqStr << std::endl;

        // 2.对请求request反序列化
        // 2.1 得到一个结构化的请求对象
        Request req;
        if (!req.deserialize(reqStr))
            return;
        // 3.计算处理业务 req._x, req._op, req._y --- 业务逻辑
        // 3.1 得到一个结构化的响应
        Response resp;
        cal(req, resp);

        std::string respStr;
        resp.serialize(&respStr);
        // 5.然后发送响应给客户端
        // 5.1构建成为一个完整的报文
        std::string send_string = enLength(respStr);
        conn->_outbuffer += enLength(respStr);

        // 发回去
        // std::cout << "--------------result: " << send_string << std::endl;
        // 这里不能直接发送给客户端，用户无法保证发送条件式就绪的 什么是写发送事件就绪？--发送缓冲区有空间了
        // 对于epoll 读事件要常设，写事件按需处理
        if(conn->_sender) conn->_sender(conn);
        // 如果没有发完，需要对对应的sock开启对写事件的关心；如果发完了，我们要关闭对写事件的关系
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    uint16_t port = atoi(argv[1]);

    unique_ptr<tcpserver::TcpServer> tcpsvr(new tcpserver::TcpServer(calculat, port));

    tcpsvr->InitServer();
    tcpsvr->Dispatch();

    return 0;
}