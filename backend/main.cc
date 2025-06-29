#include "controllers/LoginApi/Login.h"
#include "controllers/RegisterApi/Register.h"
#include <drogon/HttpAppFramework.h>

int main() {
  drogon::app().loadConfigFile("../config.json");
  drogon::app().run();
  return 0;
}