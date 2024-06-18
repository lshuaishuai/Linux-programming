#include "pollServer.hpp"
#include "err.hpp"

#include <memory>

void Usage(const char* proc)
{
    std::cout << "\n\t" << "Usage: " << proc << std::endl;
}

std::string HandleRequest(const std::string& request)
{
    return request;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    std::unique_ptr<poll_ns::PollServer> pollsvr(new poll_ns::PollServer(HandleRequest, atoi(argv[1])));

    pollsvr->initServer();
    pollsvr->start();

    return 0;
}