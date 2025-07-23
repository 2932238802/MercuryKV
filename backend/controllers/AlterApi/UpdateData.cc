#include "Alter.h"

using namespace drogon;
using namespace drogon_model::mercury;
using namespace drogon::orm;
using namespace common;

/**
 * @brief AlterData 修改数据
    1. put 请求
    2. 实现一下 修改数据的函数 更新数据库
    3. 请求发来的内容 应该有
    4. 前端需要的内容 {kv_id,user_id,key_input,value_input,tag_name}
    5. 返回的内容 应该有
 *
 * @param req
 * @param callback
 * @param id
 */
drogon::Task<drogon::HttpResponsePtr> Alter::AlterData(HttpRequestPtr req)
{

    // 获取请求体中的 JSON 对象
    auto jsonobject = req->getJsonObject();
    auto db_client = app().getDbClient();

    Service::AlterDataService::ptr service =
        Service::AlterDataServiceFactory::MakeService(db_client);

    try
    {
        auto ret = co_await service->AlterData(*jsonobject);
        const Json::Value &tags_json = (*jsonobject)["tags"];
        Json::Value success_resp;
        Json::Value data;
        success_resp["code"] = ret.code;
        success_resp["message"] = "信息更新成功";
        data["key_input"] = ret.key_input;
        data["value_input"] = ret.value_input;
        data["kv_id"] = ret.kv_id;
        data["tags"] = tags_json;
        data["updated_at"] = ret.updated_at;
        success_resp["data"] = data;
        auto res = drogon::HttpResponse::newHttpJsonResponse(success_resp);
        MY_LOG_SUC("信息更新成功");
        MY_LOG_SUC("信息更新成功!", success_resp);
        co_return res;
    }
    catch (const common::BaseException &e)
    {
        Json::Value error;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);

        if (dynamic_cast<const common::DBOperatorWrong *>(&e))
        {
            error["code"] = 500;
            error["message"] = e.what();
            resp->setStatusCode(drogon::k500InternalServerError);
            co_return resp;
        }
        else if (dynamic_cast<const common::UnkownWrong *>(&e))
        {
            error["code"] = 501;
            error["message"] = e.what();
            resp->setStatusCode(drogon::k500InternalServerError);
            co_return resp;
        }
        else
        {
            error["code"] = 400;
            error["message"] = e.what();
            resp->setBody(error.toStyledString());
            resp->setStatusCode(drogon::k400BadRequest);
            co_return resp;
        }
    }
}