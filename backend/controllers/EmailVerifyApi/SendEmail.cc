#include "EmailVerify.hpp"

drogon::Task<drogon::HttpResponsePtr> EmailVerify::SendEmail(drogon::HttpRequestPtr req,
                                                             const std::string &email_address)
{
    Json::Value ret;
    try
    {
        auto redis_client = co_await drogon::app().getRedisClient()->newTransactionCoro();
        std::string code = common::RandStr(6);
        std::string rediskey = "verify:code:" + email_address;
        co_await redis_client->execCommandCoro("SET %s %s EX 600", rediskey.c_str(), code.c_str());
        Service::EmailService::SendEmail(email_address, code);
        ret["message"] = "验证码已经发";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
        co_return resp;
    }
    catch (const drogon::nosql::RedisException &e)
    {
        LOG_ERROR << "Redis 使用失败! " << e.what();
        ret["error"] = "Internal server error (Redis).";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(drogon::k500InternalServerError);
        co_return resp;
    }
    catch (const std::exception &e)
    {
        // 捕获其他所有异常
        LOG_ERROR << "An exception occurred: " << e.what();
        ret["error"] = "Internal server error.";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(drogon::k500InternalServerError);
        co_return resp;
    }
}
