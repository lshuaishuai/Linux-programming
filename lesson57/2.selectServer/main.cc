#include "selectServer.hpp"
#include "err.hpp"
#include <memory>

using namespace std;
using namespace select_ns;

static void usage(std::string proc)
{
    std::cerr << "Usage:\n\t" << proc << " prot" << std::endl; 
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    unique_ptr<SelectServer> svr(new SelectServer(atoi(argv[1])));

    svr->initServer();
    svr->start();

    return 0;
}