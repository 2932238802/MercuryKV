
#pragma once
#include "MyLog.hpp"
#include "type.hpp"
#include <drogon/drogon.h>
#include <drogon/plugins/Plugin.h>
#include <stdexcept>

namespace Plugin
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

class SmptPlugin : public drogon::Plugin<SmptPlugin>
{
  public:
    SmptPlugin() = default;
    void initAndStart(const Json::Value &config) override;
    void shutdown() override;
    const SmtpConfig &getConfig() const;

  private:
    SmtpConfig stmp_config;
    bool is_confured;
};

} // namespace Plugin
