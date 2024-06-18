#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <jsoncpp/json/json.h>

#define SEP " "
#define SEP_LEN strlen(SEP) // 不能使用sizeof()
#define LINE_SEP "\r\n"
#define LINE_SEP_LEN strlen(LINE_SEP)

enum
{
    OK = 0,
    DIV_ZERO,
    MOD_ZERO,
    OP_ERROR
};

// "_exitcode _result" -> "content_len"\r\n"_exitcode _result"\r\n
// "_x _op _y\r\n" -> "content_len"\r\n"__x _op _y"\r\n
std::string enLength(const std::string &text)
{
    std::string send_string = std::to_string(text.size());
    send_string += LINE_SEP;
    send_string += text;
    send_string += LINE_SEP;

    return send_string;
}

// "content_len"\r\n"_x _op _y"\r\n -> "_x _op _y"
bool deLength(const std::string &package, std::string *text)
{
    auto pos = package.find(LINE_SEP);
    if (pos == std::string::npos)
        return false;
    std::string text_len_string = package.substr(0, pos);
    int text_len = std::stoi(text_len_string);
    *text = package.substr(pos + LINE_SEP_LEN, text_len);
    return true;
}

// 没有人规定我们的网络通信的时候 只能有一种协议
// 我们如何让系统知道我们用的哪一种协议？
// "协议编号"\r\n"content_len"\r\n"_exitcode _result"\r\n

class Request
{
public:
    Request()
        : _x(0), _y(0), _op(0)
    {
    }

    Request(int x, int y, char op)
        : _x(x), _y(y), _op(op)
    {
    }

    // 序列化 自己写、现成的
    bool serialize(std::string *out)
    {
#ifdef MYSELF
        *out = "";
        // 结构化 -> "_x _op _y\r\n" 一个请求就一行
        std::string x_string = std::to_string(_x);
        std::string y_string = std::to_string(_y);
        *out = x_string;
        *out += SEP;
        *out += _op;
        *out += SEP;
        *out += y_string;
#else
        Json::Value root;
        root["first"] = _x;
        root["second"] = _y;
        root["oper"] = _op;

        Json::FastWriter writer;
        // Json::StyledWriter writer;

        *out = writer.write(root); // 序列化
#endif
        return true;
    }

    // 反序列化
    bool deserialize(const std::string &in)
    {
#ifdef MYSELF

        // "_x _op _y\r\n" -> 结构化数据
        auto left = in.find(SEP);
        auto right = in.rfind(SEP); // 从右往前
        if (left == std::string::npos || right == std::string::npos)
            return false;
        if (left == right)
            return false;
        std::string x_string = in.substr(0, left);                                            // 前闭后开区间
        std::string y_string = in.substr(right + SEP_LEN, strlen(in.c_str()) - LINE_SEP_LEN); // 前闭后开区间
        if (right - (left + SEP_LEN) != 1)
            return false;
        _op = in[left + SEP_LEN];
        if (x_string.empty())
            return false;
        if (y_string.empty())
            return false;
        _x = std::stoi(x_string);
        _y = std::stoi(y_string);
#else
        Json::Value root;
        Json::Reader reader;

        reader.parse(in, root);

        _x = root["first"].asInt();
        _y = root["second"].asInt();
        _op = root["oper"].asInt();
#endif
        return true;
    }

public:
    // "_x _op _y" 约定
    int _x;
    int _y;
    char _op;
};

class Response
{
public:
    Response()
        : _exitcode(0), _result(0)
    {
    }

    // 序列化
    bool serialize(std::string *out)
    {
#ifdef MYSELF
        *out = "";
        std::string ec_string = std::to_string(_exitcode);
        std::string res_string = std::to_string(_result);
        *out += ec_string;
        *out += SEP;
        *out += res_string;
        return true;
#else
        Json::Value root;
        root["exitcode"] = _exitcode;
        root["result"] = _result;

        Json::FastWriter writer;
        *out = writer.write(root);
#endif
    }

    // 反序列化
    bool deserialize(const std::string &in)
    {
#ifdef MYSELF
        // "_exitcode result"
        auto mid = in.find(SEP);
        if (mid == std::string::npos)
            return false;
        std::string ec_string = in.substr(0, mid);
        std::string res_string = in.substr(mid + SEP_LEN);

        if (ec_string.empty() || res_string.empty())
            return false;

        _exitcode = std::stoi(ec_string);
        _result = std::stoi(res_string);

#else
        Json::Value root;
        Json::Reader reader;

        reader.parse(in, root);

        _exitcode = root["exitcode"].asInt();
        _result = root["result"].asInt();

#endif
        return true;
    }

public:
    int _exitcode; // 0计算成功 !0表示计算失败，具体是多少，定好标准
    int _result;   // 计算结果
};

bool ParseOnePackage(std::string &inbuffer, std::string *text)
{

    auto pos = inbuffer.find(LINE_SEP);
    if (pos == std::string::npos)
        return false;
    std::string text_len_string = inbuffer.substr(0, pos);
    int text_len = std::stoi(text_len_string); // 得到有效载荷的长度
    int total_len = text_len_string.size() + 2 * LINE_SEP_LEN + text_len; // 一个完整报文的长度
  
    if (total_len > inbuffer.size()) // 缓冲区还没有读到一个完整的报文
        return false;

    // 至少有一个完整的报文
    *text = inbuffer.substr(0, total_len); // "content_len"\r\n"_exitcode _result"\r\n
    inbuffer.erase(0, total_len);

    return true;
}