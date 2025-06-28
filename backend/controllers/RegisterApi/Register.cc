#include "Register.h"
#include "MyCrypt.hpp"
#include "MyJWT.hpp"
#include "MyLog.hpp"
#include "Users.h"
#include <drogon/HttpTypes.h>
#include <drogon/orm/Exception.h>
#include <json/value.h>

using namespace MyLogNS;
using namespace MyCryptNS;
using namespace drogon_model::mercury;
using namespace drogon::orm;
using namespace drogon;
using namespace MyJWTNS;

void Register::HandleRegister(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto registerjson = req->getJsonObject();

  if (!registerjson) {
    // TODO: 后面统一规定这里 输出什么
    MY_LOG_ERROR("");
  }

  //   TODO: 和前端内容在检查一遍
  std::string username = (*registerjson).get("username", "").asString();
  std::string passward = (*registerjson).get("password", "").asString();
  std::string email = (*registerjson).get("email", "").asString();

  MY_LOG_SUC("user_name:", username, "\n", "password", passward, "\n", "email",
             email, "\n");

  try {
    //   信息获取完毕之后输出一下
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
    mapper.insert(user);

    // 生成对应的user_id 和 token
    std::string user_id = std::to_string(user.getValueOfId());
    std::string token = MyJWT::GetJWT(user_id);

    // 响应和回馈
    Json::Value result;
    // TODO 前端看一眼

    result["message"] = "User registered successfully";
    result["token"] = token;
    auto resp = HttpResponse::newHttpJsonResponse(result);
    resp->setStatusCode(k201Created);
    MY_LOG_SUC("用户插入成功", username);

    callback(resp);

  }
  // 500 表示服务器内部错误
  catch (const DrogonDbException &e) {
    MY_LOG_ERROR("用户插入数据库失败");

    Json::Value error;

    // TODO 前端看一眼
    error["code"] = 500;
    error["message "] = "数据库插入失败!";
    auto resp = HttpResponse::newHttpJsonResponse(error);
    resp->setStatusCode(drogon::k500InternalServerError);
    callback(resp);
  }
}
