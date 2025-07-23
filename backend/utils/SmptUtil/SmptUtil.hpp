
#pragma once
#include "MyLog.hpp"
#include "Type.hpp"
#include <drogon/drogon.h>
#include <drogon/plugins/Plugin.h>
#include <stdexcept>

namespace Utils
{

using namespace common;
struct SmtpConfig
{
    std::string username;
    std::string password;
    std::string from_address;
    std::string from_name;
    unsigned short port;
    std::string host; // 主机名字
};

class SmptUtil
{
  public:
    SmptUtil(const Json::Value &config);
    const SmtpConfig &GetConfig() const;
    bool IsConfigured() const
    {
        return is_confured;
    }

  private:
    SmtpConfig stmp_config;
    bool is_confured = false;
};

} // namespace Utils
