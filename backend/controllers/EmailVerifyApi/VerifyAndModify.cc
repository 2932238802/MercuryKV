#include "EmailVerify.hpp"
#include "MyLog.hpp"

drogon::Task<drogon::HttpResponsePtr> EmailVerify::VerifyAndModify(drogon::HttpRequestPtr req)
{
    try
    {
        // 一个是负责 修改数据库的内容 一个是验证一下是不是对的
        auto req_json = req->getJsonObject();
        MY_LOG_INF(*req_json);
        std::string username = (*req_json)["username"].asString();
        std::string password = (*req_json)["password"].asString();
        std::string email = (*req_json)["email"].asString();
        std::string user_id_string = (*req_json)["user_id"].asString();
        std::string user_code = (*req_json)["user_code"].asString();
        int64_t user_id = std::stoll(user_id_string);

        auto verify_ret = co_await Service::EmailService::Verify(email, user_code);
        if (verify_ret.code == 400)
        {
            Json::Value error;
            error["message"] = verify_ret.message;
            auto res = drogon::HttpResponse::newHttpJsonResponse(error);
            res->setStatusCode(drogon::k400BadRequest);
            co_return res;
        }
        auto modify_service = Service::AlterPersonInfoFactory::MakeService();
        auto ret = co_await modify_service->AlterInfo(username, password, email, user_id);
        Json::Value res_json;
        res_json["code"] = ret.code;
        res_json["message"] = ret.message;
        res_json["username"] = ret.username;
        res_json["email"] = ret.email;
        auto res = drogon::HttpResponse::newHttpJsonResponse(res_json);
        co_return res;
    }
    catch (const drogon::nosql::RedisException &e)
    {
        Json::Value error;
        MY_LOG_ERROR(e.what());
        error["message"] = e.what();
        auto res = drogon::HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(drogon::k500InternalServerError);
        co_return res;
    }
    catch (const common::RequestWrong &e) // 请求错误
    {
        Json::Value error;
        MY_LOG_ERROR(e.what());
        error["message"] = e.what();
        auto res = drogon::HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(drogon::k500InternalServerError);
        co_return res;
    }
    catch (const common::DBOperatorWrong &e)
    {
        Json::Value error;
        MY_LOG_ERROR(e.what());
        error["message"] = e.what();
        auto res = drogon::HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(drogon::k500InternalServerError);
        co_return res;
    }
    catch (const std::exception &e)
    {
        Json::Value error;
        error["message"] = e.what();
        auto res = drogon::HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(drogon::k500InternalServerError);
        co_return res;
    }
}
