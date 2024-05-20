#include "HttpServer.hpp"
#include <memory>

using namespace std;
using namespace server;

void Usage(std::string proc)
{
    cerr << "Usage:\n\t" << proc << " port\r\n\r\n";
}

std::string suffixToDesc(const std::string suffix)
{
    std::string ct = "Content-Type: ";
    if (suffix == ".html")
        ct += "text/html";
    else if (suffix == ".jpg")
        ct += "application/x-jpg";
    ct += "\r\n";
    return ct;
}
// 1. 服务器和网页分离，html
// 2. url -> / : web根目录
// 3. 我们要正确的给客户端返回资源类型，我们首先要自己知道！所有的资源都有后缀！！
bool Get(const HttpRequest &req, HttpResponse &resp)
{
    // if(req.path == "test.py")
    // {
    //     //建立进程间通信，pipe
    //     //fork创建子进程，execl("/bin/python", test.py)
    //     // 父进程，将req.parm 通过管道写给某些后端语言，py，java，php等语言
    // }
    // if(req.path == "/search")
    // {
    //     // req.parm
    //     // 使用我们自己写的C++的方法，提供服务
    // }
    // for test
    cout << "----------------------http start---------------------------" << endl;
    cout << req.inbuffer << std::endl;
    std::cout << "method: " << req.method << std::endl;
    std::cout << "url: " << req.url << std::endl;
    std::cout << "httpversion: " << req.httpversion << std::endl;
    std::cout << "path: " << req.path << std::endl;
    std::cout << "suffix: " << req.suffix << std::endl;
    std::cout << "size: " << req.size << "字节" << std::endl;
    cout << "----------------------http end---------------------------" << endl;

    std::string respline = "HTTP/1.1 200 OK\r\n";
    std::string respheader = suffixToDesc(req.suffix);
    if (req.size > 0)
    {
        respheader += "Content-Length: ";
        respheader += std::to_string(req.size);
        respheader += "\r\n";
    }

    // respheader += "Location: https://www.qq.com/\r\n";
    std::string respblank = "\r\n";
    // std::string body = "<html lang=\"en\"><head><meta charset=\"UTF-8\"><title>for test</title><h1>hello world</h1></head><body><p>北京交通广播《一路畅通》“交通大家谈”节目，特邀北京市交通委员会地面公交运营管理处处长赵震、北京市公安局公安交通管理局秩序处副处长 林志勇、北京交通发展研究院交通规划所所长 刘雪杰为您解答公交车专用道6月1日起社会车辆进出公交车道须注意哪些？</p></body></html>";

    std::string body;
    body.resize(req.size+1);
    if (!Util::readFile(req.path, (char*)body.c_str(), req.size))
    {
        Util::readFile(html_404, (char*)body.c_str(), req.size); // 一定能成功
    }
    
    resp.outbuffer += respline;
    resp.outbuffer += respheader;
    resp.outbuffer += respblank;

    cout << "----------------------http response start---------------------------" << endl;
    std::cout << resp.outbuffer << std::endl;
    cout << "----------------------http response end---------------------------" << endl;

    resp.outbuffer += body;



    return true;
}

// ./httpServer 8080
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    uint16_t port = atoi(argv[1]);
    unique_ptr<HttpServer> httpsvr(new HttpServer(Get, port));

    // httpsvr->registerCb("/", Get); // 功能路由！
    // httpsvr->registerCb("/search", Search);
    // httpsrv->registerCb("/test.py", Other);
    httpsvr->initServer();
    httpsvr->start();

    return 0;
}