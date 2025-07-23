#include "ConfigManage/ConfigManage.hpp"
#include "MyLog.hpp"
#include <Poco/SharedPtr.h>
#include <csignal>
#include <drogon/drogon.h>

using namespace common;
int main(int argc, char *argv[])
{
    MyLog::GetInstance().Init("logs/mercury_kv.log");
    drogon::app().loadConfigFile("../config.json");
    Utils::ConfigManage::GetInstance().Init();
    drogon::app().run();
    return 0;
}