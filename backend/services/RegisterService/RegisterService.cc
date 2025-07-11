#include "RegisterService.hpp"
#include "Users/Users.h"

Service::RegisterService::RegisterService(drogon::orm::DbClientPtr db) : db_client(db)
{
}

drogon::Task<Service::RegisterResult> Service::RegisterService::Register(const Json::Value &req)
{
    std::string username = req.get("username", "").asString();
    std::string passward = req.get("password", "").asString();
    std::string email = req.get("email", "").asString();
    std::string salt(common::MyCrypt::GenerateSalt()); // 盐
    std::string password_hash = common::MyCrypt::Hash(passward + salt);

    if (username == "" || passward == "" || email == "")
    {
        throw RequestWrong("注册服务 请求内容不完整!");
    }

    // 完整了打印一下数据
    MY_LOG_SUC("username:", username, "\n", "password", passward, "\n", "email", email, "\n");

    try
    {
        auto trans = co_await db_client->newTransactionCoro();
        std::string sql = "INSERT INTO users (username,email,password_hash,salt) "
                          "values(\$1,\$2,\$3,\$4)";

        co_await trans->execSqlCoro(sql, username, email, password_hash, salt);

        MY_LOG_SUC("用户注册成功");

        std::string sql_foruserid = "SELECT * from users where email = \$1";
        auto user_sql = co_await trans->execSqlCoro(sql_foruserid, email);

        if (user_sql.empty())
        {
            throw UnkownWrong("服务器发生未知错误");
        }
        drogon_model::mercury::Users user(user_sql[0]);

        std::string user_id = std::to_string(user.getValueOfUserId());
        std::string token = common::MyJWT::GetJWT(user_id);
        RegisterResult ret;
        ret.code = 201;
        ret.email = email;
        ret.message = "注册成功";
        ret.token = token;
        ret.user_id = std::stoll(user_id);
        ret.username = username;
        co_return ret;
    }
    catch (drogon::orm::DrogonDbException &e)
    {
        MY_LOG_ERROR("数据库发生错误", e.base().what());
        throw DBOperatorWrong("数据库发生错误");
    }
    catch (const std::exception &e)
    {
        MY_LOG_ERROR("注册服务 发生未知错误", e.what());
        throw UnkownWrong("注册服务 发生未知错误");
    }
}