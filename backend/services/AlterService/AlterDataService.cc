

#include "AlterService/AlterDataService.hpp"
#include "type.hpp"
#include <drogon/orm/Exception.h>
#include <exception>

drogon::Task<Service::AlterDataReturn> Service::AlterDataService::AlterData(const Json::Value &json)
{
    if (!json)
    {
        throw Service::RequestWrong("请求内容或者格式错误");
    }
    if (!(json).isMember("user_id") || !(json)["user_id"].isInt64() || !(json).isMember("kv_id") ||
        !(json)["kv_id"].isInt64() || !(json).isMember("key_input") ||
        !(json)["key_input"].isString() || !(json).isMember("value_input") ||
        !(json)["value_input"].isString() || !(json).isMember("tags") || !(json)["tags"].isArray())
    {

        throw Service::RequestWrong("请求内容不完整 或者 内部类型错误");
    }

    int64_t kv_id = (json)["kv_id"].asInt64(); // kv的id
    auto key_input = (json)["key_input"].asString();
    auto value_input = (json)["value_input"].asString();
    auto updated_at = trantor::Date::now();
    auto tag_name = (json)["tag_name"].asString();
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
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        throw Service::DBOperatorWrong("数据库操作错误", e.base().what());
    }
    catch (const std::exception &e)
    {
        throw Service::UnkownWrong("处理修改函数中 发生未知错误", e.what());
    }
}