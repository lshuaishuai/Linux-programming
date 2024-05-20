#pragma once

#include <iostream>
#include <string>
#include <fstream>

class Util
{
public:
    static std::string getOnline(std::string buffer, const std::string& sep)
    {
        auto pos = buffer.find(sep);
        if(pos == std::string::npos) return "";
        std::string sub = buffer.substr(0, pos);
        buffer.erase(0, sub.size()+sep.size()); // 将首航从buffe中去掉
        return sub; 
    }
};

static bool readFile(const std::string resource, std::string *out)
{
    std::ifstream in(resource);
    if(!in.is_open()) return false; // 资源不存在
    
    std::string line;
    while(std::getline(in, line));
    {
        *out = line;
    }

    in.close();
}