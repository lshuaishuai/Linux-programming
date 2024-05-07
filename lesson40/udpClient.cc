#include "udpClient.hpp"
#include <memory>

using namespace std;
using namespace Client;

void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " server_ip server_port\n\n" << endl;
}   

// ./udpClient server_ip server_port
int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    string serverIp = argv[1]; // 必须要知道把消息发给谁
    uint16_t serverPort = atoi(argv[2]);

    std::unique_ptr<udpClient> ucli(new udpClient(serverIp, serverPort));

    ucli->initClient();
    ucli->run();

    return 0;
}