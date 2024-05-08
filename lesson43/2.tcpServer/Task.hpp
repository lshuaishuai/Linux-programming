#pragma once

#include <iostream>
#include <functional>
#include <cstdio>
#include <string>

using namespace std;

void serviceIO(int sock)
{
    char buffer[1024];
    while(true)
    {
        ssize_t n = read(sock, buffer, sizeof(buffer)-1);
        if(n > 0)
        {
            // 目前把独到的数据当成字符串 截至目前
            buffer[n] = 0;
            cout << "recv message:" << buffer << endl;

            string outBuffer = buffer;
            outBuffer += "  server[echo]";

            write(sock, outBuffer.c_str(), outBuffer.size());
        }
        else if(n == 0)
        {
            // 代表client退出
            logMessage(NORMAL, "client quit, me too!");
            break;
        }
    }           
    close(sock);
} 

class Task
{
    using func_t = std::function<void(int)>;

public:
    Task()
    {}
    
    Task(int sock, func_t func)
        :_sock(sock)
        ,_callback(func)
    {}

    void operator()()
    {
        _callback(_sock);
    }

private:
    int _sock;
    func_t _callback;
};

const std::string oper = "+-*/%";

int myMath(int x, int y, char op)
{
    int result = 0;
    switch(op)
    {
        case '+':
            result = x + y;
            break;
        case '-':
            result = x - y;
            break;
        case '*':
            result = x * y;
            break;
        case '/':
        {
            if(y == 0){
                 std::cerr << "div zero error!" << std::endl;
                 result = -1;
            }
            else result = x / y;
        }
        break;
        case '%':
        {
            if(y == 0){
                 std::cerr << "mod zero error!" << std::endl;
                 result = -1;
            }
            else result = x % y;
        }
        break;
        default:
        // do nothing
            break;
    }
    return result;
}