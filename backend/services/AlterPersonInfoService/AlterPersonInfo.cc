#include "AlterPersonInfo.hpp"
#include "MyLog.hpp"

drogon::Task<Service::AlterPersonInfoReturn>
Service::AlterPersonInfo::AlterInfo(const std::string &username, const std::string &password,
                                    const std::string &email, int64_t user_id)
{
    // 1. 数据库的修改
    // 2. 返回是不是修改成功
    try
    {
        MY_LOG_INF("username: ", username, "password: ", password, "email: ", email,
                   "user_id: ", user_id);
        auto db_client = drogon::app().getDbClient();
        auto trans = co_await db_client->newTransactionCoro();
        std::string salt(common::MyCrypt::GenerateSalt());
        std::string password_hash = common::MyCrypt::Hash(password + salt);
        std::string sql =
            "UPDATE users SET username = \$1, password_hash = \$2, email = \$3 ,salt = "
            "\$4 WHERE user_id = \$5;";

        auto res = co_await trans->execSqlCoro(sql, username, password_hash, email, salt, user_id);

        if (res.affectedRows() > 0)
        {
            Service::AlterPersonInfoReturn ret;
            ret.message = "修改成功!";
            ret.code = 200;
            ret.username = username;
            ret.email = email;
            co_return ret;
        }
        else
        {
            Service::AlterPersonInfoReturn ret;
            throw RequestWrong("请求内容不正确 修改失败");
        }
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        throw DBOperatorWrong("数据库操作失败!");
    }
}