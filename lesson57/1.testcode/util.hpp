#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

void SetNonBlock(int fd)
{
    int fl = fcntl(fd, F_GETFL); // 将当前的文件描述符的属性取出来(这是一个位图)
    if(fl < 0)
    {
        std::cerr << "fcntl: " << strerror(errno) << std::endl;
        return;
    }
    fcntl(fd, F_SETFL, fl | O_NONBLOCK); // 然后再使用F_SETFL将文件描述符设置回去. 设置回去的同时, 加上一个O_NONBLOCK参数
}

void printLog()
{
    std::cout << "this is a log" << std::endl;
}

void download()
{
    std::cout << "this is a download" << std::endl;
}

void executeSql()
{
    std::cout << "this is a executeSql" << std::endl;
}
