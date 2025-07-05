#include <drogon/HttpAppFramework.h>
int main()
{
    drogon::app().loadConfigFile("../config.json");
    // drogon::app().setLogLevel(trantor::Logger::kTrace);
    drogon::app().run();
    return 0;
}
