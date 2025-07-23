#include "ConfigManage.hpp"
#include "MyLog.hpp"
#include <drogon/HttpAppFramework.h>
#include <json/value.h>

Utils::ConfigManage &Utils::ConfigManage::GetInstance()
{
    static ConfigManage cm;
    return cm;
}

// 初始化一次
void Utils::ConfigManage::Init()
{
    if (is_init == true)
    {
        return;
    }
    auto app_config_ = drogon::app().getCustomConfig();
    MY_LOG_INF(app_config_);

    if (!app_config_.empty())
    {
        config_ = app_config_;
        is_init = true;
        MY_LOG_INF("配置管理器初始化成功");
    }
    else
    {
        MY_LOG_ERROR("未找到 custom_config 配置节");
    }
}

// 获取 配置
const Json::Value &Utils::ConfigManage::GetConfig()
{
    if (is_init == true)
    {
        MY_LOG_SUC("配置文件信息正确!");
        return config_;
    }
    else
    {
        throw common::ConfigWrong("配置文件信息错误");
    }
}

const Json::Value &Utils::ConfigManage::GetSmtpConfig()
{
    const auto &config = GetConfig();
    if (!config.isMember("smtp"))
    {
        throw common::ConfigWrong("未找到 SMTP 配置");
    }
    return config["smtp"];
}
