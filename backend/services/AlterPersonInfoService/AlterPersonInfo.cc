#include "AlterPersonInfo.hpp"
#include "MyCrypt.hpp"
#include "type.hpp"
#include <cstdint>
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Exception.h>

drogon::Task<Service::AlterPersonInfoReturn>
Service::AlterPersonInfo::AlterInfo(const std::string &username, const std::string &password,
                                    const std::string &email, int64_t user_id)
{
    // 1. 数据库的修改
    // 2. 返回是不是修改成功
    try
    {
        auto db_client = drogon::app().getDbClient();
        auto trans = co_await db_client->newTransactionCoro();
        std::string salt(common::MyCrypt::GenerateSalt());
        std::string password_hash = common::MyCrypt::Hash(password + salt);
        std::string sql =
            "UPDATE users SET username = \$1, password_hash = \$2, email = \$3 ,salt = "
            "\$4 WHERE id = \$5;";

        auto res = co_await trans->execSqlCoro(sql, username, password_hash, email, salt, user_id);

        if (!res.empty())
        {
            Service::AlterPersonInfoReturn ret;
            ret.message = "修改成功!";
            ret.code = 200;
            ret.username = username;
            co_return ret;
        }
        else
        {
            Service::AlterPersonInfoReturn ret;
            MY_LOG_ERROR("请求内容不正确 修改失败");
            throw Service::RequestWrong("请求内容不正确 修改失败");
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        Service::DBOperatorWrong("数据库操作失败!");
    }
}