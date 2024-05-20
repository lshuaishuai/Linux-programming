#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <fcntl.h>

class Util
{
public:
    // XXXX XXX XXX\r\nYYYYY
    static std::string getOneLine(std::string &buffer, const std::string &sep)
    {
        auto pos = buffer.find(sep);
        if(pos == std::string::npos) return "";
        std::string sub = buffer.substr(0, pos);
        //buffer.erase(0, sub.size()+sep.size());
        return sub;
    }
    static bool readFile(const std::string resource, char *buffer, int size)
    {
        std::ifstream in(resource, std::ios::binary);
        if(!in.is_open()) return false; // resource not found

        in.read(buffer, size);
        // std::string line;
        // while(std::getline(in, line))
        // {
        //     *out += line;
        // }

        in.close();
        return true;
    }
};