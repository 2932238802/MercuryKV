#pragma once

#include <drogon/HttpController.h>
#include <functional>

class Login : public drogon::HttpController<Login> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Login::HandleLogin, "/api/login", {drogon::Post});
  METHOD_LIST_END

  void
  HandleLogin(const drogon::HttpRequestPtr &req,
              std::function<void(const drogon::HttpResponsePtr &)> &&callback);
};
