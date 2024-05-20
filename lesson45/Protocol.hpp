#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Util.hpp"

const std::string sep = "\r\n";
const std::string default_root = "./wwwroot";
const std::string home_page = "index.html";  // 默认首页
const std::string html_404 = "wwwroot/404.html";

class HttpRequest
{
public:
    HttpRequest()
    {}
    ~HttpRequest()
    {}

    void parse()
    {
        // 1.从inbuffer中拿到第一行 分隔符\r\n
        std::string line = Util::getOnline(inbuffer, sep);
        if(line.empty()) return;
        
        // 2.从请求行读取三个字段
        std::cout << "line: " << line << std::endl;
        std::stringstream ss(line);
        ss >> method >> url >> httpVerson; // 以流的方式按照空格分割字符串

        // 3.添加web默认路径
        path = default_root; // ./wwwroot
        path += url;  // .wwwroot/a/b/c/.html  若url维'/' 则path= .wwwroot/
        if(path[path.size()-1] == '/') path += home_page;
    }

public:
    std::string inbuffer;
    // std::string reqLine;
    // std::vector<std::string> reqHeader;
    // std::string body;

    std::string method;
    std::string url;
    std::string httpVerson;
    std::string path;

  

private:


};

class HttpResponse
{
public:

    std::string outbuffer;

private:

};