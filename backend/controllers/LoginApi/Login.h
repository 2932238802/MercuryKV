#pragma once

#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>

class Login : public drogon::HttpController<Login>
{
  public:
    // drogon::Task<drogon::HttpResponsePtr> HandleLogin(const drogon::HttpRequestPtr &req);
    // drogon::Task<drogon::HttpResponsePtr> HandCheck(const drogon::HttpRequestPtr &req);
    drogon::Task<drogon::HttpResponsePtr> HandleLogin(drogon::HttpRequestPtr req);
    drogon::Task<drogon::HttpResponsePtr> HandCheck(drogon::HttpRequestPtr req);
    METHOD_LIST_BEGIN
    METHOD_ADD(Login::HandleLogin, "/login", {drogon::Post});
    METHOD_ADD(Login::HandCheck, "/checktoken", {drogon::Get, drogon::Post});
    METHOD_LIST_END
  private:
};
