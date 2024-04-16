#pragma once

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cassert>
#include <cerrno>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define NAMED_PIPE "/tmp/mypipe.106"

bool createFifo(const std::string& path)
{
    umask(0);
    int n = mkfifo(path.c_str(), 0600);  // 只允许拥有者通信
    if(n == 0) return true;
    else{
       std::cout << "erro" << errno << "err string: " << strerror(errno) << std::endl;
        return false;
    }
}

void removeFifo(const std::string & path)
{
    int n = unlink(path.c_str());
    assert(n == 0); // debug有效，release里面就无效
    (void)n;  // 不想有警告
}
