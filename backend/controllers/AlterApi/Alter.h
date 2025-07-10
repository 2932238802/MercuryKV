#pragma once
#include "AlterService/AddDataService.hpp"
#include "AlterService/AlterDataService.hpp"
#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/orm/Criteria.h>
#include <drogon/utils/coroutine.h>
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

    drogon::Task<drogon::HttpResponsePtr> AddData(const HttpRequestPtr &req);

    drogon::Task<drogon::HttpResponsePtr> AlterData(const HttpRequestPtr &req);

    void DeleteData(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    const std::string &kv_id);
};
