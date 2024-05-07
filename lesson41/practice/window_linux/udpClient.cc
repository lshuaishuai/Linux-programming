#include "udpClient.hpp"
#include <memory>

void Usage(const string& str)
{
    cout<< "Usage: \n\t" << str << " serverIp serverPort\n\n"; 
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[1]);
        exit(USAGE_ERROR);
    }

    string serverIp = argv[1];
    uint16_t serverPort = atoi(argv[2]);    

    unique_ptr<UdpClient> udpC(new UdpClient(serverIp, serverPort));

    udpC->initClient();
    udpC->runClient();

    return 0;
}