#pragma once

#include <drogon/drogon.h>
#include <functional>

class Login : public drogon::HttpController<Login> {
public:
  void
  HandleLogin(const drogon::HttpRequestPtr &req,
              std::function<void(const drogon::HttpResponsePtr &)> &&callback);
  METHOD_LIST_BEGIN
  METHOD_ADD(Login::HandleLogin, "/login", {drogon::Post});
  METHOD_LIST_END
};
