#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <stdarg.h>

using namespace std;

#define DEBUG 0
#define NORMAL 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

const char *to_levelstr(int level)
{
    switch (level)
    {
    case DEBUG:
        return "DEBUG";
    case NORMAL:
        return "NORMAL";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    case FATAL:
        return "FATAL";
    default:
        return nullptr;
    }
}

void logMessage(int level, const char *format, ...) // ... 可变参数列表
{
#define NUM 1024
    char logPreFix[NUM];
    snprintf(logPreFix, sizeof(logPreFix), "[%s][%ld][pid: %d]", to_levelstr(level), (long)time(nullptr), getpid()); // 用于格式化字符串并将其写入缓冲区
    
    char logContent[NUM];
    va_list arg;
    va_start(arg, format);

    vsnprintf(logContent, sizeof(logContent), format, arg); // 它允许用户传递一个参数列表（va_list）作为参数，而不是直接将参数传递给格式化字符串。这使得在某些情况下能够更灵活地处理参数。
    
    std::cout << logPreFix << logContent << std::endl;
}