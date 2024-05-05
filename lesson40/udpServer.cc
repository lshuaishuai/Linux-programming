#include "udpServer.hpp"
#include <memory>

using namespace std;
using namespace Server;

void handlerMessage(string clientIp, uint16_t clientPort, string message)
{
    // 就可以对message进行特定的业务处理 而不关心message怎么来的 -- server和业务逻辑解耦
}

static void Usage(string proc)
{
    cout << "Usage:\n\t" << proc << " local_ip local_port\n\n";
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);
    // string ip = argv[1];
    // std::unique_ptr<udpServer> usvr(new udpServer(port, ip));
    std::unique_ptr<udpServer> usvr(new udpServer(handlerMessage, port));

    usvr->initServer();
    usvr->start();

    return 0;
}