#include "Login.h"
#include "MyCrypt.hpp"
#include "MyJWT.hpp"
#include "MyLog.hpp"
#include "Users/Users.h"
#include <drogon/HttpResponse.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>
#include <json/json.h>
#include <json/value.h>
#include <string>
using namespace MyLogNS;
using namespace MyJWTNS;
using namespace MyCryptNS;
using namespace drogon_model::mercury;
using namespace drogon::orm;
using namespace drogon;

// ---------------------------------------------------------------------------------------------------------

void Login::HandleLogin(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto requestjson = req->getJsonObject();

  // requestjson 请求体
  // error 返回错误类型
  // k400BadRequest 客户端口的错误码
  if (!requestjson) {
    Json::Value error;
    error["message"] = "无效的请求,缺少JSON数据";
    auto resp = HttpResponse::newHttpJsonResponse(error);
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }

  // 账号密码 获取
  // TODO: √
  std::string username = (*requestjson).get("username", "").asString();
  std::string password = (*requestjson).get("password", "").asString();

  auto db_client = app().getDbClient();
  Mapper<Users> mapper(db_client);

  Criteria criteria("username", CompareOperator::EQ, username);

  mapper.findOne(
      criteria,
      [callback, password](const Users &user) {
        std::string salt = user.getValueOfSalt();
        auto password_stored = user.getPasswordHash();
        if (MyCrypt::VerifyPassword(password, salt, *password_stored)) {
          std::string token =
              MyJWT::GetJWT(std::to_string(user.getValueOfUserId()));

          Json::Value result;
          result["message"] = "登录成功!";
          result["code"] = 200;
          result["token"] = token;

          auto resp = HttpResponse::newHttpJsonResponse(result);
          callback(resp);
        } else {
          Json::Value error;
          // TODO: 前端处理 message
          error["message"] = "密码错误";
          error["code"] = 400;
          auto resp = HttpResponse::newHttpJsonResponse(error);
          resp->setStatusCode(k401Unauthorized);
          callback(resp);
        }
      },

      [callback](const DrogonDbException &e) {
        if (typeid(e) == typeid(UnexpectedRows)) {
          Json::Value error;
          // TODO: 前端处理 message
          error["message"] = "用户名或密码错误";
          error["code"] = 400;
          auto resp = HttpResponse::newHttpJsonResponse(error);
          resp->setStatusCode(k401Unauthorized);
          callback(resp);
        } else {
          // TODO: 前端处理 message
          MY_LOG_ERROR("数据库查询异常: ", e.base().what());

          // 形成json文件
          Json::Value error;
          error["message"] = "服务器内部错误，请稍后重试";
          error["code"] = 500;
          auto resp = HttpResponse::newHttpJsonResponse(error);
          resp->setStatusCode(k500InternalServerError);

          callback(resp);
        }
      });
}

// ---------------------------------------------------------------------------------------------------------
/**
 * @brief 处理前端的token请求 这样 登录之后 就不用在登陆了 直接进入
 *
 * @param req
 * @param callback
 */
void Login::HandCheck(
    const drogon::HttpRequestPtr &req,
    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
  // 从 URL 查询参数中获取 "token"
  std::string header = req->getHeader("Authorization");
  const std::string bearer_prefix = "Bearer ";
  std::string token = "";

  if (!header.empty() && header.find(bearer_prefix, 0) == 0) {
    token = header.substr(bearer_prefix.length());
  }
  // 检查 token 是否为空
  if (token.empty()) {
    Json::Value error;
    error["code"] = 400;
    error["message"] = "无效的请求,缺少 token 参数";
    auto resp = HttpResponse::newHttpJsonResponse(error);
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }
  // 获取token 然后验证一下
  auto user_id_optional = MyJWT::Verufyjwt(token);
  if (user_id_optional) {
    // 生成token
    std::string user_id = *user_id_optional;
    Json::Value res_info;
    res_info["message"] = "欢迎回来!";
    res_info["user_id"] = user_id;
    res_info["code"] = 200;
    auto res = HttpResponse::newHttpJsonResponse(res_info);

    // 日志输出
    MY_LOG_INF("token:", token);
    MY_LOG_SUC("user_id:", user_id);
    MY_LOG_SUC("认证成功!");

    callback(res);
  } else {
    Json::Value error;
    error["code"] = 400;
    error["message"] = "认证失败,请重新登录";

    auto res = HttpResponse::newHttpJsonResponse(error);
    MY_LOG_ERROR("认证失败!");

    callback(res);
  }
}