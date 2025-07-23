
#include "LoginService.hpp"

using namespace common;
/**
 * @brief 初始化 数据库操作台 不用反复创建
 * @param db
 */
Service::LoginService::LoginService(drogon::orm::DbClientPtr db) : db_client(db)
{
}

/**
 * @brief 登录验证 重新封装 到Service
 * @param username 账号
 * @param password 密码
 * @return std::future<Service::Result>
 */
drogon::Task<Service::LoginResult> Service::LoginService::LoginVerify(const std::string &username,
                                                                      const std::string &password)
{
    drogon::orm::Mapper<drogon_model::mercury::Users> mapper(db_client);
    drogon::orm::Criteria criteria("username", drogon::orm::CompareOperator::EQ, username);

    try
    {
        auto result =
            co_await db_client->execSqlCoro("SELECT * FROM users WHERE username = \$1", username);

        if (result.empty())
        {
            MY_LOG_WARN("登录尝试失败: 用户 '" + username + "' 不存在");
            throw AuthException("登录尝试失败: 用户 '" + username + "' 不存在");
        }

        if (result.size() > 1)
        {
            MY_LOG_ERROR("数据异常: 用户名 '" + username + "' 存在多个记录");
            throw DBOperatorWrong("数据异常: 用户名 '" + username + "' 存在多个记录");
        }

        drogon_model::mercury::Users user(result[0]);
        auto password_stored_opt = user.getPasswordHash();
        if (!password_stored_opt)
        {
            MY_LOG_ERROR("数据异常: 用户 '" + username + "' 缺少密码哈希");
            throw DBOperatorWrong("数据异常: 用户 '" + username + "' 缺少密码哈希");
        }

        std::string salt = user.getValueOfSalt();
        std::string password_stored = *password_stored_opt;
        if (common::MyCrypt::VerifyPassword(password, salt, password_stored))
        {
            std::string token = common::MyJWT::GetJWT(std::to_string(user.getValueOfUserId()));
            LoginResult result;
            result.code = 200;
            result.email = user.getValueOfEmail();
            result.token = token;
            result.user_id = user.getValueOfUserId();
            result.message = "登录成功!";
            MY_LOG_SUC("登录成功");
            co_return result;
        }
        else
        {
            MY_LOG_ERROR("密码错误");
            throw AuthException("密码错误");
        }
    }
    catch (const drogon::orm::UnexpectedRows &e)
    {
        MY_LOG_ERROR("没有找到对应的用户!");
        throw AuthException("没有找到对应的用户!");
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        MY_LOG_ERROR("数据库操作失败!");
        throw DBOperatorWrong("数据库操作失败!");
    }
}