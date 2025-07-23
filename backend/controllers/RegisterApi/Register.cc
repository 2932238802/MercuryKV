#include "Register.h"

using namespace common;

// ----- ----- ----- ----- -----
/**
 * @brief 这个就是简单的注册函数
 *
 * @param req
 * @return drogon::Task<drogon::HttpResponsePtr>
 */
drogon::Task<drogon::HttpResponsePtr> Register::HandleRegister(HttpRequestPtr req)
{

    auto jsonobject = req->getJsonObject();
    if (!jsonobject)
    {
        Json::Value error_resp;
        error_resp["code"] = 400;
        error_resp["message"] = "请求体为空或非JSON格式";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error_resp);
        resp->setStatusCode(drogon::k400BadRequest);
        co_return resp;
    }

    auto db_client = app().getDbClient();
    auto service = Service::RegisterServiceFactory::MakeService(db_client);

    try
    {
        auto ret_from_service = co_await service->Register(*jsonobject);
        Json::Value message;

        message["message"] = ret_from_service.message;
        message["code"] = ret_from_service.code;
        message["email"] = ret_from_service.email;
        message["token"] = ret_from_service.token;
        message["user_id"] = ret_from_service.user_id;
        message["username"] = ret_from_service.username;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(message);
        co_return resp;
    }
    catch (const Service::BaseException &e)
    {
        Json::Value error;
        if (dynamic_cast<const Service::DBOperatorWrong *>(&e))
        {
            error["code"] = 500;
            error["message"] = "服务器内部错误，请稍后重试";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(drogon::k500InternalServerError);
            co_return resp;
        }
        else if (dynamic_cast<const Service::UnkownWrong *>(&e))
        {
            error["code"] = 501;
            error["message"] = "服务器发生未知错误";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(drogon::k500InternalServerError);
            co_return resp;
        }
    }
}
