#include "epollServer.hpp"
#include <memory>

static void usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << " prot" << std::endl;
}

std::string echo(const std::string& message)
{
    return "I am epollserver, " + message;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    uint16_t port = atoi(argv[1]);

    unique_ptr<epoll_ns::EpollServer>epollsvr(new epoll_ns::EpollServer(echo, port));

    epollsvr->initServer();
    epollsvr->start();
    

    return 0;
}