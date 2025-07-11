#pragma once
#include "AddDataService.hpp"
#include "MyLog.hpp"
#include <trantor/utils/Date.h>

drogon::Task<Service::AddDataReturn> Service::AddDataService::AddData(const Json::Value &json)
{
    if (!json)
    {
        throw Service::RequestWrong("请求体数据错误");
    }
    if (!(json).isMember("user_id") || !(json)["user_id"].isInt64() ||
        !(json).isMember("key_input") || !(json)["key_input"].isString() ||
        !(json).isMember("value_input") || !(json)["value_input"].isString() ||
        !(json).isMember("tags") || !(json)["tags"].isArray())
    {
        throw Service::RequestWrong("请求体数据缺失");
    }

    uint64_t user_id = (json)["user_id"].asInt64();
    std::string key_input = (json)["key_input"].asString();
    std::string value_input = (json)["value_input"].asString();
    const Json::Value &tags_json = (json)["tags"];

    // 日志输出
    MY_LOG_INF("user_id:", user_id, "\nkey_input:", key_input, "\nvalue_input", value_input, "/n",
               tags_json);

    if (key_input == "" || value_input == "")
    {
        MY_LOG_ERROR("key_input value_input tag_name 内容为空");
        throw Service::RequestWrong("key_input value_input tag_name 内容为空");
    }

    // 插入对应的表
    auto trans = co_await db_client->newTransactionCoro();

    try
    {
        // 中文转换
        Json::Value value_input_json(value_input);
        Json::StreamWriterBuilder writer;
        std::string value_input_for_db = Json::writeString(writer, value_input_json);

        auto now = trantor::Date::now();

        std::string sql = "INSERT INTO kv_store(user_id,key_input,value_input,updated_at) "
                          "values(\$1,\$2,\$3,\$4) RETURNING kv_id";

        auto result_kv =
            co_await trans->execSqlCoro(sql, user_id, key_input, value_input_for_db, now);

        if (result_kv.empty())
        {
            throw Service::DBOperatorWrong("插入kv_store失败");
        }
        int64_t kv_id = result_kv[0]["kv_id"].as<int64_t>();

        for (const auto &tag_json : tags_json)
        {
            if (!tag_json.isString())
                continue;
            std::string tag_name = tag_json.asString();
            if (tag_name.empty())
                continue;
            int64_t tag_id;
            std::string sql =
                "SELECT tag_id from tags where user_id = \$1 And tag_name = \$2 limit 1";
            auto result = co_await trans->execSqlCoro(sql, user_id, tag_name);
            if (!result.empty())
            {
                tag_id = result[0]["tag_id"].as<int64_t>();
            }
            else
            {
                std::string insert_tag_sql =
                    "INSERT INTO tags (user_id, tag_name) VALUES (\$1, \$2) RETURNING tag_id";
                auto result_tag_insert =
                    co_await trans->execSqlCoro(insert_tag_sql, user_id, tag_name);

                if (result_tag_insert.empty())
                {
                    throw Service::DBOperatorWrong("插入新tag失败");
                }
                tag_id = result_tag_insert[0]["tag_id"].as<int64_t>();
            }
            std::string insert_kta_sql =
                "INSERT INTO kv_tag_association (kv_id, tag_id) VALUES (\$1, \$2)";
            co_await trans->execSqlCoro(insert_kta_sql, kv_id, tag_id);
        }
        AddDataReturn ret;
        ret.code = 201;
        ret.key_input = key_input;
        ret.value_input = value_input;
        ret.kv_id = kv_id;
        ret.updated_at = now.toDbStringLocal();
        MY_LOG_INF("kv_id:", ret.kv_id);
        co_return ret;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        MY_LOG_ERROR("DrogonDbException Captured: ", e.base().what());
        throw Service::DBOperatorWrong("数据库出错");
    }
    catch (const std::exception &e)
    {
        MY_LOG_ERROR("exception Captured: ", e.what());
        throw Service::UnkownWrong("插入数据时候 发生未知错误");
    }
}