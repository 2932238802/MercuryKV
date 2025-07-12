#include "EmailVerify.hpp"

drogon::Task<drogon::HttpResponsePtr>
EmailVerify::VerifyAndModify(const drogon::HttpRequestPtr &req, const std::string &user_code,
                             const std::string &email)
{
    // 一个是负责 修改数据库的内容 一个是验证一下是不是对的
    try
    {
        auto verify_ret = co_await Service::EmailService::Verify(email, user_code);

        if (verify_ret.code == 400)
        {
            // 认证失败了
            Json::Value error;
            MY_LOG_ERROR("验证失败 请输入正确的验证码");
            error["message"] = "验证失败 请输入正确的验证码";
            auto res = drogon::HttpResponse::newHttpJsonResponse(error);
            res->setStatusCode(drogon::k400BadRequest);
            co_return res;
        }
        // 验证成功了

        // code = 200;
    }
    catch (const drogon::nosql::RedisException &e)
    {
        Json::Value error;
        MY_LOG_ERROR("验证修改数据失败 服务器redis错误");
        error["message"] = "验证修改数据失败 服务器redis错误";
        auto res = drogon::HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(drogon::k500InternalServerError);
        co_return res;
    }
    catch (const std::exception &e)
    {
        Json::Value error;
        error["message"] = "验证修改数据失败 服务器未知错误";
        auto res = drogon::HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(drogon::k500InternalServerError);
        co_return res;
    }
}
