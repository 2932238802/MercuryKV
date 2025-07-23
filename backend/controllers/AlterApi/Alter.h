#pragma once
#include "AlterService/AddDataService.hpp"
#include "AlterService/AlterDataService.hpp"
#include "AlterService/DeleteDataService.hpp"
#include "MyLog.hpp"
#include "Type.hpp"
#include <cstdint>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Exception.h>
#include <drogon/utils/coroutine.h>
#include <exception>
#include <json/value.h>
#include <optional>
#include <string>
#include <trantor/utils/Date.h>

using namespace drogon;
class Alter : public drogon::HttpController<Alter>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(Alter::AddData, "/adddata", Post);
    METHOD_ADD(Alter::AlterData, "/alterdata", Put);
    METHOD_ADD(Alter::DeleteData, "/deletedata/{id}", Delete);
    METHOD_LIST_END

    drogon::Task<drogon::HttpResponsePtr> AddData(HttpRequestPtr req);

    drogon::Task<drogon::HttpResponsePtr> AlterData(HttpRequestPtr req);

    drogon::Task<drogon::HttpResponsePtr> DeleteData(HttpRequestPtr req, const std::string &kv_id);
};
