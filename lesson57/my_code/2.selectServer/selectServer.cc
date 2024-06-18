#include "selectServer.hpp"
#include "err.hpp"

#include <memory>

void usage(const std::string& proc)
{
    std::cerr << "Usage:\n\t" << proc << "\n\t" << std::endl;
}

string transc(const string& request)
{
    return request;
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }
    
    uint16_t port = atoi(argv[1]);
    std::unique_ptr<select_ns::SelectServer> select_svr(new select_ns::SelectServer(transc, port));

    select_svr->initServer();
    select_svr->start();

    return 0;
}