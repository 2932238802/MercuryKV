#pragma once

#include "MyLog.hpp"
#include "Type.hpp"
#include <json/value.h>
namespace Utils
{
using namespace common;
class ConfigManage
{
  public:
    static ConfigManage &GetInstance();
    void Init();
    const Json::Value &GetConfig();
    const Json::Value &GetSmtpConfig();

  private:
    Json::Value config_; // 这个用来获取 config.json
    bool is_init = false;
};

} // namespace Utils