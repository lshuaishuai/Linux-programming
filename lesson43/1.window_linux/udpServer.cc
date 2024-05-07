#include "udpServer.hpp"
#include <memory>
#include <unordered_map>
#include <fstream>
#include <signal.h>

using namespace std;
using namespace Server;

// demo1
void handlerMessage(int sockfd, string clientIp, uint16_t clientPort, string message)
{
    // 就可以对message进行特定的业务处理 而不关心message怎么来的 -- server和业务逻辑解耦
    string response_message = message;
    

    // 开始返回
    struct sockaddr_in client;
    bzero(&client, sizeof client);

    client.sin_family = AF_INET;
    client.sin_port = htons(clientPort);
    client.sin_addr.s_addr = inet_addr(clientIp.c_str()); 

    sendto(sockfd, response_message.c_str(), response_message.size(), 0, (struct sockaddr*)&client, sizeof(client));
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

    std::unique_ptr<udpServer> usvr(new udpServer(handlerMessage, port));

    usvr->initServer();
    usvr->start();

    return 0;
}