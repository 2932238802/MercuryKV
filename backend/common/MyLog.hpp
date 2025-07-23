#pragma once
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
namespace common
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
    // 获取单例实例
    static MyLog &GetInstance()
    {
        static MyLog instance;
        return instance;
    }

    void Init(const std::string &logfilepath)
    {
        logfile.open(logfilepath, std::ios::app); // 以追加模式打开
        if (!logfile.is_open())
        {
            std::cerr << "打开指定文件失败" << logfilepath << std::endl;
        }
    }

    template <class... Args> void Log(const char *file, int line, G grade, Args... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);

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

        std::stringstream ss_content;
        (ss_content << ... << args);
        std::string content_str = ss_content.str();

        if (logfile.is_open())
        {
            logfile << "[" << time_buffer << "] " << "[" << grade_str << "] " << "["
                    << GetFile(file) << ":" << line << "] >>> " << content_str << std::endl;
        }

        std::cout << "[" << time_buffer << "]" << " ";
        std::cout << "[" << color_str << grade_str << RESET << "]" << " ";
        std::cout << "[" << BLUE << GetFile(file) << ":" << line << RESET << " @LosAngelous]>>> ";
        std::cout << content_str << std::endl;
    }

  private:
    MyLog() = default;
    ~MyLog()
    {
        if (logfile.is_open())
        {
            logfile.close();
        }
    }
    MyLog(const MyLog &) = delete;
    MyLog &operator=(const MyLog &) = delete;
    std::ofstream logfile;
    std::mutex mutex_;
};

#define MY_LOG_INF(...) MyLog::GetInstance().Log(__FILE__, __LINE__, G::INF, ##__VA_ARGS__)
#define MY_LOG_WARN(...) MyLog::GetInstance().Log(__FILE__, __LINE__, G::WAR, ##__VA_ARGS__)
#define MY_LOG_ERROR(...) MyLog::GetInstance().Log(__FILE__, __LINE__, G::ERR, ##__VA_ARGS__)
#define MY_LOG_SUC(...) MyLog::GetInstance().Log(__FILE__, __LINE__, G::SUC, ##__VA_ARGS__)

} // namespace common
