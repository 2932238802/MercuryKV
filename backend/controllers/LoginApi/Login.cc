#include "Login.h"
#include "models/Users.h"
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <json/value.h>

#include "common/MyLog.hpp"

using namespace MyLogNS;

void Login::asyncHandleHttpRequest(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  // 获取请求体
  // 使用引用避免拷贝
  // 接受到了json情况 存储到数据库
  auto requestjson = req->getJsonObject();
  const auto &requestjson_copy = *requestjson;
  std::string user_name = requestjson_copy["user_name"].asString();
  std::string password = requestjson_copy["password"].asString();

  //   接收不到json的情况
  if (requestjson == nullptr) {
    Json::Value error;
    // 错误处理 是接受不到json文件的情况
    error["message"] = "客户端信息出现问题,json处理失败!";
    auto reshttp = HttpResponse::newHttpJsonResponse(error);

    // 设置状态密码
    reshttp->setStatusCode(drogon::k400BadRequest);
    callback(reshttp);
    return;
  }

  // 答复json
  Json::Value res;
  res["message"] = "";
  auto reshttp = HttpResponse::newHttpJsonResponse(res);

  // 获取一个映射的类
  auto db_handle = drogon::app().getDbClient();
  drogon::orm::Mapper<drogon_model::mercury::Users> mapper(db_handle);

  mapper.findOne(
      drogon::orm::Criteria("user_name", drogon::orm::CompareOperator::EQ,
                            user_name),
      [](const drogon_model::mercury::Users &user) {

      },
      []() {

      }

  )
}

// -------------------------
// sql yujv
// std::string sql =
//     "select * from users where user_name = ? , password_hash = ?";
// db_handle->execSqlAsync(
//     sql,
//     [&](const drogon::orm::Result &result) {
//       if (result.size() == 0) {
//         // 说明查询不到对应的数据
//         reshttp->setStatusCode(drogon::k401Unauthorized);
//       } else {
//         reshttp->setStatusCode(drogon::k200OK);
//       }

//       callback(reshttp);
//       return;
//     },
//     // 失败回调处理
//     [&](const drogon::orm::DrogonDbException &e) {
//       LOG_ERROR("登录失败!");

//       Json::Value error;

//       error["code"] = 500;
//       error["message"] = "服务器内部错误!";
//     },
//     user_name, password);

// ------------------------------
// TEST
// if (user_name == "1111" && password == "1111111") {
//   Json::Value res;

//   // TODO: 前端
//   res["message"] = "登录成功!";
//   auto reshttp = HttpResponse::newHttpJsonResponse(res);

//   reshttp->setStatusCode(drogon::k200OK);

//   callback(reshttp);
//   return;
// }
// ------------------------------