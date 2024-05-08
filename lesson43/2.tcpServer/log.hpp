#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <stdarg.h>
#include <ctime>
#include <unistd.h>

using namespace std;

#define LOG_NORMAL "log.txt" // 0 1 2
#define LOG_ERR "log.err"    // 3 4

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
    //  [日志等级] [时间戳/时间] [pid] [message]
    //  [WARNING] [2023-05-11 18:09:08] [123] [创建socket失败]

    
#define NUM 1024
    char logPreFix[NUM];
    snprintf(logPreFix, sizeof(logPreFix), "[%s][%ld][pid: %d]", to_levelStr(level), (long int)time(nullptr), getpid());

    char logContent[NUM];
    va_list arg;
    va_start(arg, format);

    vsnprintf(logContent, sizeof(logContent), format, arg);
    
    // cout << logPreFix << logContent << endl;

    // 将日志文件写入文件中
    FILE *log = fopen(LOG_NORMAL, "a");
    FILE *err = fopen(LOG_ERR, "a"); // 追加写入
    if(log != nullptr && err != nullptr)
    {
        FILE *curr = nullptr;
        if(level == DEBUG || level == NORMAL || level == WARNING) curr = log;
        if(level == ERROR || level == FATAL) curr = err;
        if(curr) fprintf(curr, "%s%s\n", logPreFix, logContent);

        fclose(log);
        fclose(err);
    }

}