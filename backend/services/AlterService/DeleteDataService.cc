

#include "AlterApi/Alter.h"
#include "type.hpp"
#include <cstdint>
#include <drogon/orm/Exception.h>
#include <exception>
drogon::Task<Service::DeleteDataReturn>
Service::DeleteDataService::DeleteData(const std::string &kv_str)
{
    // 这里进行具体的删除工作
    if (kv_str.empty())
    {
        throw RequestWrong("请求内容不完整 尤其是对应kv_id空缺");
    }
    // 转换成整型
    try
    {
        int64_t kv_id = std::stoll(kv_str);
        std::string sql_delete_from_kv_store = "DELETE FROM kv_store WHERE kv_id = \$1";
        std::string sql_delete_from_kv_tag_association =
            "DELETE FROM kv_tag_association WHERE kv_id = \$1";
        auto trans = co_await db_client->newTransactionCoro();
        auto ret_sql_from_kv_store = co_await trans->execSqlCoro(sql_delete_from_kv_store, kv_id);
        auto ret_sql_from_kv_tag_association =
            co_await trans->execSqlCoro(sql_delete_from_kv_tag_association, kv_id);
        if (ret_sql_from_kv_store.affectedRows() == 0)
        {
            throw RequestWrong("没有对应的请求数据 内容错误");
        }
        Service::DeleteDataReturn ret;
        ret.code = 200;
        ret.message = "数据删除成功";
        co_return ret;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        MY_LOG_ERROR("DBOperatorWrong Captured: ", e.base().what());
        throw DBOperatorWrong(e.base().what());
    }
    catch (const std::invalid_argument &e)
    {
        MY_LOG_ERROR("UnkownWrong Captured: ", e.what());
        throw UnkownWrong(e.what());
    }
}