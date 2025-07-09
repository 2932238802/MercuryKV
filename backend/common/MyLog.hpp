#pragma once

#include <cstdio>
#include <cstring>
#include <ctime>

#include <iostream>
#include <string>

namespace MyLogNS
{
#define RESET "\033[0m"
#define BLACK "\033[30m"   /* Black */
#define RED "\033[31m"     /* Red */
#define GREEN "\033[32m"   /* Green */
#define YELLOW "\033[33m"  /* Yellow */
#define BLUE "\033[34m"    /* Blue */
#define MAGENTA "\033[35m" /* Magenta */
#define CYAN "\033[36m"    /* Cyan */
#define WHITE "\033[37m"   /* White */
enum class G
{
    SUC = 1,
    INF,
    WAR,
    ERR,
};

inline const char *GetFile(const char *str)
{
    if (!str)
        return "UnknownFile";                 // 安全检查
    const char *pos_fwd = strrchr(str, '/');  // 正斜杠
    const char *pos_bwd = strrchr(str, '\\'); // 反斜杠

    const char *pos = pos_fwd;
    if (pos_bwd && (!pos_fwd || pos_bwd > pos_fwd))
    {
        pos = pos_bwd;
    }

    if (pos)
    {
        return pos + 1;
    }
    return str;
}

class MyLog
{
  public:
    template <class... Args> static void Log(const char *file, int line, G grade, Args... args)
    {
        std::string grade_str = "";
        std::string color_str = "";
        switch (grade)
        {
        case G::ERR:
            grade_str = "ERR";
            color_str = RED;
            break;
        case G::INF:
            grade_str = "INF";
            break;
        case G::WAR:
            grade_str = "WAR";
            color_str = MAGENTA;
            break;
        case G::SUC:
            grade_str = "SUC";
            color_str = GREEN;
            break;
        default:
            grade_str = "UNKWN";

            break;
        }

        time_t now = time(nullptr);
        struct tm *tm_now = localtime(&now);
        char time_buffer[80];
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_now);

        std::cout << "[" << time_buffer << "]" << " ";
        std::cout << "[" << color_str << grade_str << RESET << "]" << " ";
        std::cout << "[" << BLUE << GetFile(file) << ":" << line << RESET << " @LosAngelous]>>> ";
        (std::cout << ... << args) << std::endl;
    }
};

#define MY_LOG_INF(...) MyLog::Log(__FILE__, __LINE__, G::INF, ##__VA_ARGS__)
#define MY_LOG_WARN(...) MyLog::Log(__FILE__, __LINE__, G::WAR, ##__VA_ARGS__)
#define MY_LOG_ERROR(...) MyLog::Log(__FILE__, __LINE__, G::ERR, ##__VA_ARGS__)
#define MY_LOG_SUC(...) MyLog::Log(__FILE__, __LINE__, G::SUC, ##__VA_ARGS__)

} // namespace MyLogNS
