#pragma once

#include "MyCrypt.hpp"
#include "MyJWT.hpp"
#include "MyLog.hpp"
#include "Register.h"
#include "RegisterService/RegisterService.hpp"
#include "Users/Users.h"
#include "type.hpp"
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpSimpleController.h>
#include <drogon/HttpTypes.h>
#include <drogon/orm/Exception.h>
#include <json/value.h>
using namespace drogon;

class Register : public drogon::HttpController<Register>
{
  public:
    drogon::Task<drogon::HttpResponsePtr> HandleRegister(HttpRequestPtr req);
    METHOD_LIST_BEGIN
    METHOD_ADD(Register::HandleRegister, "/register", {drogon::Post});
    METHOD_LIST_END
};
