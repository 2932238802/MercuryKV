#include "Alter.h"

/**
 * @brief 删除数据
 *
 * @param req
 * @param callback
 * @param kv_id
 */
drogon::Task<drogon::HttpResponsePtr> Alter::DeleteData(HttpRequestPtr req,
                                                        const std::string &kv_str)
{

    auto db_client = app().getDbClient();

    Service::DeleteDataService::ptr service =
        Service::DeleteDataServiceFactory::MakeService(db_client);

    try
    {
        auto ret = co_await service->DeleteData(kv_str);
        Json::Value res_json;
        res_json["code"] = ret.code;
        res_json["message"] = ret.message;
        auto res = drogon::HttpResponse::newHttpJsonResponse(res_json);
        co_return res;
    }
    catch (const common::BaseException &e)
    {
        Json::Value error;
        if (dynamic_cast<const common::DBOperatorWrong *>(&e))
        {
            error["code"] = 500;
            error["message"] = e.what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(drogon::k500InternalServerError);
            co_return resp;
        }
        else if (dynamic_cast<const common::UnkownWrong *>(&e))
        {
            error["code"] = 501;
            error["message"] = e.what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(drogon::k500InternalServerError);
            co_return resp;
        }
        else if (dynamic_cast<const common::RequestWrong *>(&e))
        {
            error["code"] = 400;
            error["message"] = e.what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(drogon::k500InternalServerError);
            co_return resp;
        }
    }
}
