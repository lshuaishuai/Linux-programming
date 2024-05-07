#include "udpServer.hpp"
#include <memory>

using namespace std;

void Usage(const string& str)
{
    cout << "Usage:\n\t" << str << " localIp localPort \n\n";
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGEERROR);
    }
    // string ip = argv[1];
    uint16_t port = atoi(argv[1]);
    unique_ptr<UdpServer> udpS(new UdpServer(port));

    udpS->initServer();
    udpS->start();

    return 0;
}