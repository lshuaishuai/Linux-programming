#include "udpServer.hpp"
#include <memory>
#include <unordered_map>
#include <fstream>
#include <signal.h>
#include "OnlineUser.hpp"

using namespace std;
using namespace Server;

OnlineUser onlineUser;

// demo3
void routeMessage(int sockfd, string clientIp, uint16_t clientPort, string message)
{
    if(message == "online") onlineUser.addUser(clientIp, clientPort);
    if(message == "offline") onlineUser.delUser(clientIp, clientPort);

    if(onlineUser.isOnline(clientIp, clientPort)) 
    {
        // 消息路由
        onlineUser.broadcastMessage(sockfd, clientIp, clientPort, message);
        
    }
    else
    {
        struct sockaddr_in client;
        bzero(&client, sizeof client);

        client.sin_family = AF_INET;
        client.sin_port = htons(clientPort);
        client.sin_addr.s_addr = inet_addr(clientIp.c_str()); 
        string response_message = "您还没有上线,请先上线运行:online";
        sendto(sockfd, response_message.c_str(), response_message.size(), 0, (struct sockaddr*)&client, sizeof(client));

    }
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
    string ip = argv[1];

    std::unique_ptr<udpServer> usvr(new udpServer(routeMessage, port));


    usvr->initServer();
    usvr->start();

    // debugPrint();

    return 0;
}