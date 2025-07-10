#pragma once
#include "MyLog.hpp"
#include "stdexcept"
#include <sstream>

namespace Service
{
using namespace common;

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
template <typename... Args> class BaseException : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;

  public:
    std::string build_message(Args &&...args)
    {
        std::stringstream ss;
        // C++17 折叠表达式：将所有参数依次送入 stringstream
        // (ss << ... << args) 是一种写法，下面的写法更通用，可以处理分隔符
        (ss << ... << std::forward<Args>(args));
        std::string message = ss.str();
        MY_LOG_ERROR(message);
        return message;
    }
};

// 服务器错误 500
class DBOperatorWrong : public BaseException
{
  public:
    DBOperatorWrong(const std::string &str) : BaseException(str)
    {
        MY_LOG_ERROR(str);
    }
};

// 认证失败 401
class AuthException : public std::runtime_error
{
  public:
    explicit AuthException(const std::string &str) : std::runtime_error(str)
    {
        MY_LOG_ERROR(str);
    }
};

// 请求错误 400
class RequestWrong : public std::runtime_error
{
  public:
    RequestWrong(const std::string &str) : std::runtime_error(str)
    {
        MY_LOG_ERROR(str);
    }
};

// 服务器未知错误 501
template <typename... Args> class UnKownWrong : public std::runtime_error
{
  public:
    UnKownWrong(Args &&...args) : std::runtime_error(build_message(std::forward<Args>(args)...))
    {
        MY_LOG_ERROR(std::forward<Args>(args)...);
    }
};

} // namespace Service