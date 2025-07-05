#pragma once
#include <drogon/HttpController.h>
using namespace drogon;
class Alter : public drogon::HttpController<Alter>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(Alter::AddData, "/adddata", Post);
    METHOD_ADD(Alter::AlterData, "/alterdata/{id}", Put);
    METHOD_ADD(Alter::DeleteData, "/deletedata/{id}", Delete);
    METHOD_LIST_END

    void AddData(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);

    void AlterData(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback);

    void DeleteData(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    const std::string &kv_id);
};
