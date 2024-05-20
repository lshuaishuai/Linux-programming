#include "HttpServer.hpp"
#include "Util.hpp"
#include <memory>

using namespace std;
using namespace server;

void usage(std::string proc)
{
    cerr << "Usage\r\n" << proc << "  prot\r\n\r\n";
}

// 1.将服务器与网页分离 html
// 2.url-> / : web根目录
bool Get(const HttpRequest& req, HttpResponse& resp)
{
    cout << "-----------------------http start--------------------------" << endl;
    cout << req.inbuffer << std::endl;
    std::cout << "method: " << req.method << std::endl;
    std::cout << "url: " << req.url << std::endl;
    std::cout << "httpVerson: " << req.httpVerson << std::endl;
    cout << "------------------------http end--------------------------" << endl;

    std::string resLine = "HTTP/1.1 200 OK\r\n";
    std::string respheader = "Content-Type: text/html\r\n";

    std::string respblank = "\r\n";
    // std::string body = "<html lang=\"en\"><head><title>for test</title><h1>Hello shuaishuai</h1></head><body><p>帅帅真帅帅帅杀杀杀杀杀杀杀杀杀水水水水水水水水水水水水水哇的身份VS时间看看是第三空间NSA课程授课欧卡的水水水水水水水水水水水水水水水水发v熊出没</p></body></html>";
   
    std::string body;
    if(!readFile(req.path, &body))
    {
        readFile(html_404, &body); // 一定会成功
    }

    resp.outbuffer += resLine;
    resp.outbuffer += respheader;
    resp.outbuffer += respblank;
    resp.outbuffer += body;
    return true;
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        usage(argv[0]);
        exit(0);
    }

    uint16_t port = atoi(argv[1]);
    unique_ptr<HttpServer> httpsvr(new HttpServer(Get, port));
    httpsvr->initServer();
    httpsvr->start();

    return 0;
}