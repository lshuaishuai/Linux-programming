# include "tcpServer.hpp"
#include <memory>

using namespace server;
using namespace std;

static void Usage(string proc)
{
    cout << "Usage:\n\t" << proc << " local_ip local_port\n\n";
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
    
    unique_ptr<TcpServer> tsvr(new TcpServer(port));

    tsvr->initServer();
    tsvr->start();
    
    return 0;
}