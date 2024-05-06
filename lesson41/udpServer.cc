#include "udpServer.hpp"
#include <memory>
#include <unordered_map>
#include <fstream>
#include <signal.h>

using namespace std;
using namespace Server;

const string dictTxt = "./dict.txt";
unordered_map<string, string> dict;

static bool cutString(const string& target, string* s1, string* s2, const string& sep)
{   
    auto pos = target.find(sep); 
    if(pos == string::npos) return false;
    *s1 = target.substr(0, pos); // pos为:的下标
    *s2 = target.substr(pos + sep.size());
    return true;
}

// 将文件中的内容加载到容器中
static void initDict()
{
    ifstream in(dictTxt, ios::binary);
    if(!in.is_open())
    {
        cerr << "open file " << dictTxt << " error" << endl;
        exit(OPEN_ERR);
    }

    string line;
    string key, value;
    while(getline(in, line))
    {
        if(cutString(line, &key, &value, ":"));
            dict.insert(make_pair(key, value));
    }

    in.close();

    cout << "load dict success" << endl;
}

void reload(int signo)
{
    (void)signal;
    initDict();
}

static void debugPrint()
{
    for(auto& dt : dict)
    {
        cout << dt.first << " # " << dt.second << endl;
    }
}

// demo1
void handlerMessage(int sockfd, string clientIp, uint16_t clientPort, string message)
{
    // 就可以对message进行特定的业务处理 而不关心message怎么来的 -- server和业务逻辑解耦
    string response_message;
    auto iter = dict.find(message);
    if(iter == dict.end()) response_message = "unknown";
    else response_message = iter->second;

    // 开始返回
    struct sockaddr_in client;
    bzero(&client, sizeof client);

    client.sin_family = AF_INET;
    client.sin_port = htons(clientPort);
    client.sin_addr.s_addr = inet_addr(clientIp.c_str()); 

    sendto(sockfd, response_message.c_str(), response_message.size(), 0, (struct sockaddr*)&client, sizeof(client));
}

// demo2
void execCommand(int sockfd, string clientIp, uint16_t clientPort, string cmd)
{
    // 1. cmd解析 ls -a -l
    // 2.如果必要 可能需要fork，exec*
    if(cmd.find("rm") != string::npos || cmd.find("mv") != string::npos || cmd.find("rmdir") != string::npos)
    {
        cerr << clientIp << ":" << clientPort << " 正在做非法的操作：" << cmd << endl;
        return;
    }


    string response;
    FILE *fp = popen(cmd.c_str(), "r");
    if(fp == nullptr) response = cmd + " exec failed!";
    char line[1024];
    while(fgets(line, sizeof(line), fp))
    {
        response += line;
    }

    // 开始返回
    struct sockaddr_in client;
    bzero(&client, sizeof client);

    client.sin_family = AF_INET;
    client.sin_port = htons(clientPort);
    client.sin_addr.s_addr = inet_addr(clientIp.c_str()); 

    sendto(sockfd, response.c_str(), response.size(), 0, (struct sockaddr*)&client, sizeof(client));


    pclose(fp);
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

    signal(2, reload);  // 热加载 不需要重启就可以往文件中添加数据  
    initDict();

    // std::unique_ptr<udpServer> usvr(new udpServer(port, ip));
    // std::unique_ptr<udpServer> usvr(new udpServer(handlerMessage, port));
    std::unique_ptr<udpServer> usvr(new udpServer(execCommand, port));

    usvr->initServer();
    usvr->start();

    // debugPrint();

    return 0;
}