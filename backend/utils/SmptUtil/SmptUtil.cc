#include "SmptUtil.hpp"
#include <drogon/drogon.h>
#include <drogon/plugins/Plugin.h>
#include <json/value.h>
#include <stdexcept>

Utils::SmptUtil::SmptUtil(const Json::Value &config)
{
    MY_LOG_INF("准备开始初始化 stmp 配置信息");
    stmp_config.host = config.get("host", "").asString();
    stmp_config.port = static_cast<unsigned short>(config.get("port", "").asUInt());
    stmp_config.from_address = config.get("from_address", "").asString();
    stmp_config.from_name = config.get("from_name", "").asString();
    stmp_config.username = config.get("username", "").asString();
    stmp_config.password = config.get("password", "").asString();

    if (!stmp_config.host.empty() && !stmp_config.username.empty())
    {
        is_confured = true;
        MY_LOG_SUC("stmp 配置初始化成功");
        MY_LOG_SUC("username: ", stmp_config.username);
        MY_LOG_SUC("password: ", stmp_config.password);
        MY_LOG_SUC("from_address: ", stmp_config.from_address);
        MY_LOG_SUC("from_name: ", stmp_config.from_name);
        MY_LOG_SUC("host: ", stmp_config.host);
        MY_LOG_SUC("port: ", stmp_config.port);
    }
    else
    {
        MY_LOG_WARN("配置信息 不完整");
    }
}

const Utils::SmtpConfig &Utils::SmptUtil::GetConfig() const
{
    if (!is_confured)
    {
        throw std::runtime_error("配置 stmp 信息的时候发生未知错误");
    }
    return stmp_config;
}
