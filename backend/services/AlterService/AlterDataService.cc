

#include "AlterService/AlterDataService.hpp"
#include "MyLog.hpp"
#include "type.hpp"
#include <cstdint>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Exception.h>
#include <exception>
#include <string>

drogon::Task<Service::AlterDataReturn> Service::AlterDataService::AlterData(const Json::Value &json)
{
    if (!json)
    {
        throw Service::RequestWrong("请求内容或者格式错误");
    }
    if (!(json).isMember("user_id") || !(json).isMember("kv_id") || !(json).isMember("key_input") ||
        !(json)["key_input"].isString() || !(json).isMember("value_input") ||
        !(json)["value_input"].isString() || !(json).isMember("tags") || !(json)["tags"].isArray())
    {
        throw Service::RequestWrong("请求内容不完整 或者 内部类型错误");
    }
    std::string kv_id_str = (json)["kv_id"].asString(); // kv的id
    int64_t kv_id = std::stoll(kv_id_str);
    auto key_input = (json)["key_input"].asString();
    auto value_input = (json)["value_input"].asString();
    auto updated_at = trantor::Date::now();
    int64_t user_id = (json)["user_id"].asInt64();
    const Json::Value &tags_json = (json)["tags"];
    if (key_input == "" || value_input == "")
    {
        throw Service::RequestWrong("请求内容不完整 或者 内部类型错误");
    }

    // 之前的数据 之后处理
    auto trans = co_await db_client->newTransactionCoro();
    try
    {
        auto now = trantor::Date::now();

        std::string sql{"SELECT * FROM kv_store WHERE kv_id = \$1"};
        auto kv_store_result = co_await trans->execSqlCoro(sql, kv_id);

        if (kv_store_result.empty())
        {
            // 如果查询结果为空
            // 应该是前端传入参数错误 默认这个id的格式不正确 或者 传输过程中发生错误
            throw Service::UnkownWrong("可能是传输过程中发生未知错误");
        }

        // 如果不为空 那么就是对这个
        auto user_id_store = kv_store_result[0]["user_id"].as<int64_t>();
        auto value_previous = kv_store_result[0]["value_input"].as<std::string>();
        if (user_id_store != user_id)
        {
            throw AuthException("用户信息和其资源不匹配 请重新登录!");
        }
        co_await trans->execSqlCoro("UPDATE kv_store SET key_input=\$1, value_input=\$2, "
                                    "previous_value=\$3, updated_at=\$4 WHERE kv_id=\$5",
                                    key_input, value_input, value_previous, trantor::Date::now(),
                                    kv_id);
        co_await trans->execSqlCoro("DELETE FROM kv_tag_association WHERE kv_id = \$1", kv_id);
        for (const auto &tag_json : tags_json)
        {
            if (!tag_json.isString() || tag_json.asString().empty())
                continue;
            std::string tag_name = tag_json.asString();
            int64_t tag_id;
            auto result_tag = co_await trans->execSqlCoro(
                "SELECT tag_id FROM tags WHERE user_id = \$1 AND tag_name = \$2", user_id,
                tag_name);
            if (!result_tag.empty())
            {
                tag_id = result_tag[0]["tag_id"].as<int64_t>();
            }
            else
            {
                auto result_new_tag = co_await trans->execSqlCoro(
                    "INSERT INTO tags (user_id, tag_name) VALUES (\$1, \$2) RETURNING tag_id",
                    user_id, tag_name);
                tag_id = result_new_tag[0]["tag_id"].as<int64_t>();
            }
            co_await trans->execSqlCoro(
                "INSERT INTO kv_tag_association (kv_id, tag_id) VALUES (\$1, \$2)", kv_id, tag_id);
        }
        AlterDataReturn ret;
        ret.code = 200;
        ret.kv_id = kv_id;
        ret.key_input = key_input;
        ret.value_input = value_input;
        ret.updated_at = now.toDbStringLocal();
        MY_LOG_SUC("AlterDataService:\n", "ret.code:", ret.code, "\nret.kv_id:", ret.kv_id, "\n");
        co_return ret;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        MY_LOG_ERROR("DrogonDbException Captured: ", e.base().what());
        throw Service::DBOperatorWrong("数据库操作错误", e.base().what());
    }
    catch (const std::exception &e)
    {
        MY_LOG_ERROR("exception Captured: ", e.what());
        throw Service::UnkownWrong("处理修改函数中 发生未知错误", e.what());
    }
}