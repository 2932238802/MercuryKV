#pragma once
#include "AddDataService.hpp"

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
        drogon::orm::Mapper<drogon_model::mercury::KvStore> mapper_kv(trans); // kv 的表
        drogon::orm ::Mapper<drogon_model::mercury::Tags> mapper_tags(trans); // 标签的表
        drogon::orm ::Mapper<drogon_model::mercury::KvTagAssociation> mapper_kta(
            trans); // 这个是标签和kv的联系表
        drogon_model::mercury::KvStore kv;

        // 中文转换
        Json::Value value_input_json(value_input);
        Json::StreamWriterBuilder writer;
        std::string value_input_for_db = Json::writeString(writer, value_input_json);
        MY_LOG_INF("中文转化之后的值是 value_input_for_db:", value_input_for_db);
        kv.setUserId(user_id);
        kv.setKeyInput(key_input);
        kv.setValueInput(value_input_for_db);
        kv.setUpdatedAt(trantor::Date::now());
        mapper_kv.insert(kv);
        int64_t kv_id = kv.getValueOfKvId();

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
                drogon_model::mercury::Tags new_tag;
                new_tag.setUserId(user_id);
                new_tag.setTagName(tag_name);
                mapper_tags.insert(new_tag);
                tag_id = new_tag.getValueOfTagId();
            }

            drogon_model::mercury::KvTagAssociation kta;
            kta.setKvId(kv_id);
            kta.setTagId(tag_id);
            mapper_kta.insert(kta);
        }

        AddDataReturn ret;
        ret.code = 201;
        ret.key_input = key_input;
        ret.value_input = value_input;
        ret.kv_id = kv_id;
        ret.updated_at = kv.getValueOfUpdatedAt().toDbStringLocal();
        co_return ret;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        throw DBOperatorWrong("数据库出错");
    }
    catch (const std::exception &e)
    {
        throw UnKownWrong("插入数据时候 发生未知错误");
    }
}