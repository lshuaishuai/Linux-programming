#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>

#define DEBUG 0
#define NORMAL 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

const char* to_levelStr(int level)
{
    switch(level)
    {
    case DEBUG: return "DEBUG";
    case NORMAL: return "NORMAL";
    case WARNING: return "WARNING";
    case ERROR: return "ERROR";
    case FATAL: return "FATAL";
    default: return nullptr;
    }
}

void logMessage(int level, const char* format, ...)
{
#define NUM 1024
    char logPreFix[NUM];
    snprintf(logPreFix, sizeof(logPreFix), "[%s][%ld][pid: %d]: ", to_levelStr(level), (long int)time(nullptr), getpid());

    char logContent[NUM];
    va_list arg;
    va_start(arg, format);
    vsnprintf(logContent, sizeof(logContent), format, arg);
    std::cout << logPreFix << logContent << std::endl;
}