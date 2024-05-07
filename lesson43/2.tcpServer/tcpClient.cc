# include "tcpClient.hpp"

#include <memory>

using namespace std;

static void Usage(string proc)
{
    cout << "Usage:\n\t" << proc << " server_ip server_port\n\n";
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    string serverIp = argv[1];
    uint16_t serverPort = atoi(argv[2]);

    unique_ptr<TcpClient> tcli(new TcpClient(serverIp, serverPort));

    tcli->initClient();
    tcli->start();

    return 0;
}