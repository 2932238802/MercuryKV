#include "Register.h"
#include "MyCrypt.hpp"
#include "MyJWT.hpp"
#include "MyLog.hpp"
#include "Users/Users.h"
#include <drogon/HttpTypes.h>
#include <drogon/orm/Exception.h>
#include <json/value.h>

using namespace MyLogNS;
using namespace MyCryptNS;
using namespace drogon_model::mercury;
using namespace drogon::orm;
using namespace drogon;
using namespace MyJWTNS;

void Register::HandleRegister(const HttpRequestPtr &req,
                              std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto registerjson = req->getJsonObject();

    if (!registerjson)
    {
        // TODO: 后面统一规定这里 输出什么
        MY_LOG_ERROR("");
    }

    //   TODO: 和前端内容在检查一遍
    std::string username = (*registerjson).get("username", "").asString();
    std::string passward = (*registerjson).get("password", "").asString();
    std::string email = (*registerjson).get("email", "").asString();
    MY_LOG_SUC("username:", username, "\n", "password", passward, "\n", "email", email, "\n");

    try
    {
        // 信息获取完毕之后输出一下
        auto db_client = app().getDbClient();
        Mapper<Users> mapper(db_client);

        // 生成数据
        Users user;
        user.setEmail(email);
        user.setUsername(username);
        std::string salt(MyCrypt::GenerateSalt());
        std::string password_salt = MyCrypt::Hash((passward + salt));
        user.setPasswordHash(password_salt);
        user.setSalt(salt);

        // 插入数据
        try
        {
            mapper.insert(user);
        }
        catch (const drogon::orm::DrogonDbException &e)
        {
            Json::Value error;
            error["message"] = "Database insertion failed: " + std::string(e.base().what());
            auto res = HttpResponse::newHttpJsonResponse(error);
            callback(res);
            return;
        }

        // 生成对应的user_id 和 token
        std::string user_id = std::to_string(user.getValueOfUserId());
        std::string token = MyJWT::GetJWT(user_id);

        // 响应和回馈
        Json::Value result;

        // TODO 前端看一眼  一个是 message 一个是token
        // 赋值 resp
        result["message"] = "注册成功!";
        result["token"] = token;
        result["code"] = 201;
        result["user_id"] = user_id;
        result["username"] = username;

        auto resp = HttpResponse::newHttpJsonResponse(result);
        resp->setStatusCode(k201Created);
        MY_LOG_SUC("用户插入成功", username);

        // 回馈
        callback(resp);
    }
    // 500 表示服务器内部错误
    catch (const DrogonDbException &e)
    {
        // 日志输出
        MY_LOG_ERROR("用户插入数据库失败");

        // TODO 前端看一眼
        // 设置错误码
        Json::Value error;
        error["code"] = 500;
        error["message"] = "数据库插入失败!";
        auto resp = HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k500InternalServerError);

        // 回调
        callback(resp);
    }
}
