#include "ModifyPassword.h"

using namespace common;
drogon::Task<drogon::HttpResponsePtr> ModifyPassword::Modifypd(HttpRequestPtr rq)
{
    try
    {
        MY_LOG_SUC("修改密码的api接口请求正确");
        auto json = rq->getJsonObject();
        auto db = app().getDbClient();
        if (!json)
        {
            Json::Value error;
            error["message"] = "请求内容错误";
            error["code"] = 400;
            auto res = drogon::HttpResponse::newHttpJsonResponse(error);
            co_return res;
        }

        Json::Value res;
        std::string message = "";

        // json 内容没有问题 根据邮箱修改密码
        // std::string password = (*json)["password"].asString();
        std::string code = (*json)["code"].asString();
        std::string email = (*json)["email"].asString();

        auto email_service = Service::EmailServiceFactory::MakeService();
        auto res_verify = co_await email_service->Verify(email, code);
        if (res_verify.code == 400)
        {
            Json::Value error;
            error["message"] = res_verify.message;
            error["code"] = res_verify.code;
            auto res = drogon::HttpResponse::newHttpJsonResponse(error);
            co_return res;
        }

        // 验证码 没有问题
        message = "验证信息:" + res_verify.message;

        // 然后是修改密码
        auto pd_service_base =
            Service::AlterDataServiceFactory::MakeService(Service::AlterType::AlterPassword, db);

        auto pd_service = std::dynamic_pointer_cast<Service::AlterPasswordService>(pd_service_base);
        auto res_pd_service = co_await pd_service->Alter(*json);

        // 首先是修改成功了没 还有对应的异常
        message += res_pd_service.message;
        res["message"] = message;
        res["code"] = res_pd_service.code;
        auto res_ = drogon::HttpResponse::newHttpJsonResponse(res);
        co_return res_;
    }
    catch (const Service::BaseException &e)
    {
        Json::Value error;
        if (dynamic_cast<const Service::DBOperatorWrong *>(&e))
        {
            error["code"] = 500;
            error["message"] = e.what();
        }
        else if (dynamic_cast<const Service::UnkownWrong *>(&e))
        {
            error["code"] = 501;
            error["message"] = e.what();
        }
        else if (dynamic_cast<const common::RequestWrong *>(&e))
        {
            error["code"] = 400;
            error["message"] = e.what();
        }
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k500InternalServerError);
        co_return resp;
    }
}