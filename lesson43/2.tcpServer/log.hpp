#pragma once

#include <iostream>
#include <string>

#define DEBUG   0
#define NORMAL  1
#define WARNING 2
#define ERROR   3  // 出错可运行
#define FATAL   4  // 致命错误

void logMessage(int level, const std::string &message)
{
    //  [日志等级] [时间戳/时间] [pid] [message]
    //  [WARNING] [2023-05-11 18:09:08] [123] [创建socket失败]

    // 暂定
    std::cout << message << std::endl;
}