#include "Login.h"
#include "MyLog.hpp"
#include "Users.h"
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>
#include <json/json.h>
using namespace MyLogNS;
using namespace drogon_model::mercury;
using namespace drogon::orm;
using namespace drogon;
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

  std::string username = (*requestjson).get("username", "").asString();
  std::string password = (*requestjson).get("password", "").asString();

  auto db_client = app().getDbClient();
  Mapper<Users> mapper(db_client);

  Criteria criteria("user_name", CompareOperator::EQ, username);
  criteria = criteria && Criteria("password", CompareOperator::EQ, password);

  mapper.findOne(
      criteria,
      [callback](Users user) {
        Json::Value result;
        result["message"] = "登录成功!";
        auto resp = HttpResponse::newHttpJsonResponse(result);
        callback(resp);
      },
      [callback](const DrogonDbException &e) {
        if (typeid(e) == typeid(UnexpectedRows)) {
          Json::Value error;
          // TODO: 前端处理 message
          error["message"] = "用户名或密码错误";
          auto resp = HttpResponse::newHttpJsonResponse(error);
          resp->setStatusCode(k401Unauthorized);
          callback(resp);
        } else {
          // TODO: 前端处理 message
          MY_LOG_ERROR("数据库查询异常: ", e.base().what());
          Json::Value error;
          error["message"] = "服务器内部错误，请稍后重试";
          auto resp = HttpResponse::newHttpJsonResponse(error);
          resp->setStatusCode(k500InternalServerError);
          callback(resp);
        }
      });
}
