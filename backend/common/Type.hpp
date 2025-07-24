#pragma once
#include "MyLog.hpp"
#include "stdexcept"
#include <sstream>

namespace common
{
using namespace common;

template <typename... Args> inline std::string build_message(Args &&...args)
{
    std::stringstream ss;
    // 使用折叠表达式完成拼接
    (ss << ... << std::forward<Args>(args));
    return ss.str();
}

// typename...: 这里的 ... 是关键，它告诉编译器，这里接受的不是一个类型
// 而是一包（零个或多个）类型
// Args: 这是我们给这一包类型起的名字
// Args... args: 这里的 ... 再次出现，它把模板参数包 Args “解开”
// (ss << ... << std::forward<Args>(args));

// 检查父类：它会去看父类 std::runtime_error 有哪些构造函数
// 生成“隐式”构造函数：它会为 Exception
// 类生成一系列的**“转发构造函数”**
// 每一个生成的构造函数都和父类的某个构造函数有完全相同的参数列表。
// 自动调用：这些生成的构造函数内部会自动调用父类对应的构造函数
class BaseException : public std::runtime_error
{
  public:
    BaseException(const std::string &message) : std::runtime_error(message)
    {
    }
};

// 服务器错误 500 配置信息读取错误
class DBOperatorWrong : public common::BaseException
{
  public:
    template <typename... Args>
    explicit DBOperatorWrong(Args &&...args)
        : common::BaseException(build_message(std::forward<Args>(args)...))
    {
    }
};

// 服务器错误 500
class RedisOperatorWrong : public common::BaseException
{
  public:
    // 模板构造函数，可以接收任意参数
    template <typename... Args>
    explicit RedisOperatorWrong(Args &&...args)
        // 1. 调用辅助函数拼接字符串
        // 2. 调用基类构造函数，传入拼接好的字符串，基类会自动记录日志
        : common::BaseException(build_message(std::forward<Args>(args)...))
    {
    }
};

// 认证失败 401
class AuthException : public common::BaseException
{
  public:
    template <typename... Args>
    explicit AuthException(Args &&...args)
        // 1. 调用辅助函数拼接字符串
        // 2. 调用基类构造函数，传入拼接好的字符串，基类会自动记录日志
        : common::BaseException(build_message(std::forward<Args>(args)...))
    {
    }
};

// 请求错误 400
class RequestWrong : public common::BaseException
{
  public:
    template <typename... Args>
    explicit RequestWrong(Args &&...args)
        : common::BaseException(build_message(std::forward<Args>(args)...))
    {
    }
};

// 请求错误 500
class StmpWrong : public common::BaseException
{
  public:
    template <typename... Args>
    explicit StmpWrong(Args &&...args)
        : common::BaseException(build_message(std::forward<Args>(args)...))
    {
    }
};

// 服务器错误 500
class PocoNetWrong : public common::BaseException
{
  public:
    template <typename... Args>
    explicit PocoNetWrong(Args &&...args)
        : common::BaseException(build_message(std::forward<Args>(args)...))
    {
    }
};

// 服务器未知错误 501
class UnkownWrong : public common::BaseException
{
  public:
    template <typename... Args>
    explicit UnkownWrong(Args &&...args)
        : common::BaseException(build_message(std::forward<Args>(args)...))
    {
    }
};

// 服务器错误 500
class ConfigWrong : public common::BaseException
{
  public:
    template <typename... Args>
    explicit ConfigWrong(Args &&...args)
        : common::BaseException(build_message(std::forward<Args>(args)...))
    {
    }
};
} // namespace common
