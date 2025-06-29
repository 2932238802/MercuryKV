#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpSimpleController.h>

using namespace drogon;

class Register : public drogon::HttpController<Register> {
public:
  void HandleRegister(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback);
  METHOD_LIST_BEGIN
  METHOD_ADD(Register::HandleRegister, "/register", {drogon::Post});
  METHOD_LIST_END
};
