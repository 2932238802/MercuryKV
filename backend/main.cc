#include <drogon/HttpAppFramework.h>
int main()
{
    drogon::app().loadConfigFile("../config.json");
    drogon::app().setLogLevel(trantor::Logger::kInfo);
    drogon::app().run();
    return 0;
}
