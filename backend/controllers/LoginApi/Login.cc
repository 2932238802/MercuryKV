#include "Login.h"
#include "CheckTokenService/CheckTokenService.hpp"
#include "LoginService/LoginService.hpp"
#include "MyLog.hpp"
#include "Users/Users.h"
#include "type.hpp"
#include <drogon/HttpResponse.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>
#include <json/json.h>
#include <json/value.h>
#include <string>

using namespace common;
// drogon::Task<drogon::HttpResponsePtr> Login::HandleLogin(const drogon::HttpRequestPtr &req)
drogon::Task<drogon::HttpResponsePtr> Login::HandleLogin(drogon::HttpRequestPtr req)
{
    auto requestjson = req->getJsonObject();

    // requestjson 请求体
    // error 返回错误类型
    // k400BadRequest 客户端口的错误码
    if (!requestjson)
    {
        Json::Value error;
        error["message"] = "无效的请求,缺少JSON数据";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k400BadRequest);
        co_return resp;
    }

    // 账号密码 获取
    // TODO: √
    std::string username = (*requestjson).get("username", "").asString();
    std::string password = (*requestjson).get("password", "").asString();

    try
    {
        auto db_client = drogon::app().getDbClient();
        Service::LoginService::ptr ptr = Service::LoginServiceFactory::MakeService(db_client);

        // 当你调用一个返回 drogon::Task<T> 的函数时
        // 你得到的不是最终结果 T，而是这个“包裹”
        // 这个包裹告诉你：“我里面最终会有一个类型为 T 的东西，但现在可能还在路
        Service::LoginResult ret = co_await ptr->LoginVerify(username, password);

        Json::Value res_json;
        res_json["code"] = ret.code;
        res_json["email"] = ret.email;
        res_json["message"] = ret.message;
        res_json["token"] = ret.token;
        res_json["user_id"] = ret.user_id;
        res_json["username"] = username;

        auto resp = drogon::HttpResponse::newHttpJsonResponse(res_json);
        co_return resp;
    }
    catch (const Service::AuthException &e)
    {
        Json::Value error;
        error["code"] = 401;
        error["message"] = e.what();
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k401Unauthorized);
        co_return resp;
    }
    catch (const Service::DBOperatorWrong &)
    {
        Json::Value error;
        error["code"] = 500;
        error["message"] = "服务器内部错误，请稍后重试";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k500InternalServerError);
        co_return resp;
    }
    catch (const Service::UnKownWrong &e)
    {
        Json::Value error;
        error["code"] = 500;
        error["message"] = "服务器发生未知错误";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k500InternalServerError);
        co_return resp;
    }
}

// ----- ----- ----- ----- -----
/**
 * @brief 处理前端的token请求 这样 登录之后 就不用在登陆了 直接进入
 *
 * @param req
 * @param callback
 */
// drogon::Task<drogon::HttpResponsePtr> Login::HandCheck(const drogon::HttpRequestPtr &req)
drogon::Task<drogon::HttpResponsePtr> Login::HandCheck(drogon::HttpRequestPtr req)
{
    // 从 URL 查询参数中获取 "token"
    std::string header = req->getHeader("Authorization");
    auto db_client = drogon::app().getDbClient();

    Service::CheckTokenService::ptr service =
        Service::CheckTokenServiceFactory::MakeService(db_client);

    try
    {
        auto ret = co_await service->Check(std::move(header));

        Json::Value res_json;
        res_json["code"] = ret.code;
        res_json["message"] = ret.message;
        res_json["user_id"] = ret.user_id;
        res_json["username"] = ret.username;
        MY_LOG_INF("code:", ret.code, "message:", ret.message, "user_id:", ret.user_id,
                   "username:", ret.username);
        auto rep = drogon::HttpResponse::newHttpJsonResponse(res_json);
        co_return rep;
    }
    catch (const Service::DBOperatorWrong &e)
    {
        Json::Value error;
        error["code"] = 500;
        error["message"] = "库错误";
        auto rep = drogon::HttpResponse::newHttpJsonResponse(error);
        co_return rep;
    }
    catch (const Service::AuthException &e)
    {
        Json::Value error;
        error["code"] = 401;
        error["message"] = "认证失败,请重新登录";
        auto res = drogon::HttpResponse::newHttpJsonResponse(error);
        co_return res;
    }
    catch (const Service::UnKownWrong &e)
    {
        Json::Value error;
        error["code"] = 500;
        error["message"] = "服务器发生未知错误";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
        resp->setStatusCode(drogon::k500InternalServerError);
        co_return resp;
    }
}