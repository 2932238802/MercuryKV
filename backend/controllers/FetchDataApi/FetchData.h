#pragma once
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>
using namespace drogon;

class FetchData : public drogon::HttpController<FetchData>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(FetchData::Fetch, "/fetch", {drogon::Get});
    METHOD_LIST_END

    // 从数据库获取数据
    // 通过前端的用户id 所有的数据 返回数量信息
    //
    void Fetch(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
