#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <stdarg.h>
#include <ctime>
#include <unistd.h>

using namespace std;

#define DEBUG   0
#define NORMAL  1
#define WARNING 2
#define ERROR   3  // 出错可运行
#define FATAL   4  // 致命错误

const char* to_levelStr(int level)
{
    switch ((level))
    {
    case DEBUG: return "DEBUG";
    case NORMAL: return "NORMAL";
    case WARNING: return "WARNING";
    case ERROR: return "ERROR";
    case FATAL: return "FATAL";
    default: return nullptr;
    }
}

void logMessage(int level, const char* format, ...) // ... 可变参数列表 
{
#define NUM 1024
    char logPreFix[NUM];
    snprintf(logPreFix, sizeof(logPreFix), "[%s][%ld][pid: %d]", to_levelStr(level), (long int)time(nullptr), getpid());

    char logContent[NUM];
    va_list arg;
    va_start(arg, format);

    vsnprintf(logContent, sizeof(logContent), format, arg);
    
    cout << logPreFix << logContent << endl;

   
}