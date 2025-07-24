#pragma once
#include "AlterService/AlterDataService.hpp"
#include "EmailService/EmailService.hpp"
#include "KVStore/KvStore.h"
#include "MyLog.hpp"
#include "Type.hpp"
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/utils/coroutine.h>
#include <json/value.h>
#include <memory>

using namespace drogon;

class ModifyPassword : public drogon::HttpController<ModifyPassword>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(ModifyPassword::Modifypd, "/modifypd", drogon::Post);
    METHOD_LIST_END

    drogon::Task<drogon::HttpResponsePtr> Modifypd(HttpRequestPtr req);
};
