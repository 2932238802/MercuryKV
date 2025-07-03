#include "Alter.h"
#include "KVStore/KvStore.h"
#include "KV_Tag/KvTagAssociation.h"
#include "MyLog.hpp"
#include "Tags/Tags.h"
#include <cstdint>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/orm/Criteria.h>
#include <json/value.h>
#include <optional>
#include <string>
#include <trantor/utils/Date.h>

using namespace drogon;
using namespace drogon_model::mercury;
using namespace drogon::orm;
using namespace MyLogNS;

/**
 * @brief AlterData 修改数据
    1. put 请求
    2. 实现一下 修改数据的函数 更新数据库
    3. 请求发来的内容 应该有
    4. 前端需要的内容 {kv_id,user_id,key_input,value_input,tag_name}
    5. 返回的内容 应该有
 *
 * @param req
 * @param callback
 * @param id
 */
void Alter::AlterData(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback)
{

    // 获取请求体中的 JSON 对象
    auto jsonobject = req->getJsonObject();
    if (!jsonobject)
    {
        Json::Value error;
        error["message"] = "请求体错误 解析json错误!";
        auto res = HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(k400BadRequest);
        callback(res);
        return;
    }

    // 不为空
    // 不相信前端原则
    if (!(*jsonobject).isMember("user_id") || !(*jsonobject)["user_id"].isInt64() ||
        !(*jsonobject).isMember("kv_id") || !(*jsonobject)["kv_id"].isInt64() ||
        !(*jsonobject).isMember("key_input") || !(*jsonobject)["key_input"].isString() ||
        !(*jsonobject).isMember("value_input") || !(*jsonobject)["value_input"].isString() ||
        !(*jsonobject).isMember("tags") || !(*jsonobject)["tags"].isArray())
    {
        Json::Value error;
        error["error"] = "内容缺失!";
        auto res = HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(k400BadRequest);
        callback(res);
        return;
    }

    // 请求中的数据
    int64_t kv_id = (*jsonobject)["kv_id"].asInt64();
    auto key_input = (*jsonobject)["key_input"].asString();
    auto value_input = (*jsonobject)["value_input"].asString();
    auto updated_at = trantor::Date::now();
    auto tag_name = (*jsonobject)["tag_name"].asString();
    int64_t user_id = (*jsonobject)["user_id"].asInt64();
    const Json::Value &tags_json = (*jsonobject)["tags"];
    if (key_input == "" || value_input == "")
    {
        MY_LOG_ERROR("key_input value_input 为空!");

        Json::Value error;
        error["message"] = "key_input value_input 为空!";
        auto res = HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(k400BadRequest);
        callback(res);
        return;
    }

    // 数据库生成
    // 生成事务
    // 修改数据
    // 先根据主键 找到这个数据 然后 对C++ 进行修改 然后 mapper.update
    // 进行修改到数据库
    // TODO: 改一下
    auto db_client = app().getDbClient();
    auto trans = db_client->newTransaction();
    try
    {
        Mapper<KvStore> mapper(trans);
        Mapper<Tags> mapper_tags(trans);
        Mapper<KvTagAssociation> mapper_kta(trans);

        std::optional<KvStore> kv_store =
            mapper.findOne(Criteria(KvStore::Cols::_kv_id, CompareOperator::EQ, kv_id));

        if (!kv_store)
        {
            MY_LOG_ERROR("数据库中找不到对应的kv对!");

            Json::Value error;
            error["message"] = "数据库中找不到对应的kv对!";
            auto res = HttpResponse::newHttpJsonResponse(error);
            res->setStatusCode(k404NotFound);
            callback(res);
            return;
        }
        // 认证一下kv 里面的用户信息
        auto kv_to_update = kv_store.value();
        if (*(kv_to_update.getUserId()) != user_id)
        {
            MY_LOG_ERROR("用户认证失败 用户id(user_id)错误");

            Json::Value error;
            error["message"] = "user_id不存在!";
            auto res = HttpResponse::newHttpJsonResponse(error);
            // 禁止访问哈 可能是恶意攻击
            res->setStatusCode(drogon::k403Forbidden);
            callback(res);
            return;
        }

        // 获取之前的数据
        auto value_previous = kv_store->getValueOfValueInput();

        // 更新数据
        kv_to_update.setKeyInput(key_input);
        kv_to_update.setValueInput(value_input);
        kv_to_update.setPreviousValue(value_previous);
        kv_to_update.setUpdatedAt(trantor::Date::now());

        // 更新 kv数据库
        mapper.update(kv_to_update);
        mapper_kta.deleteBy(Criteria(KvTagAssociation::Cols::_kv_id, CompareOperator::EQ, kv_id));

        for (const auto &tag_json : tags_json)
        {
            if (!tag_json.isString())
                continue;
            std::string tag_name = tag_json.asString();
            if (tag_name.empty())
                continue;

            int64_t tag_id;
            auto existing_tags =
                mapper_tags.findBy(Criteria(Tags::Cols::_user_id, CompareOperator::EQ, user_id) &&
                                   Criteria(Tags::Cols::_tag_name, CompareOperator::EQ, tag_name));

            if (!existing_tags.empty())
            {
                // Tag exists, use its ID
                tag_id = existing_tags[0].getValueOfTagId();
            }
            else
            {
                // Tag does not exist, create it
                Tags new_tag;
                new_tag.setUserId(user_id);
                new_tag.setTagName(tag_name);
                mapper_tags.insert(new_tag);
                tag_id = new_tag.getValueOfTagId();
            }

            KvTagAssociation kta;
            kta.setKvId(kv_id);
            kta.setTagId(tag_id);
            mapper_kta.insert(kta);
        }
        Json::Value success_resp;
        Json::Value data;
        success_resp["code"] = 200; // 200 OK for update
        data["updated_at"] = kv_to_update.getValueOfUpdatedAt().toDbString();
        data["tags"] = tags_json; // Return the new tags array
        success_resp["data"] = data;
        auto res = HttpResponse::newHttpJsonResponse(success_resp);
        callback(res);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        MY_LOG_ERROR("数据库转换和处理失败在改变数据的过程中", e.base().what());

        // 这保证了操作的原子性
        // 会回滚
        trans->rollback();
        Json::Value err;
        err["message"] = "数据库转换和处理失败在改变数据的过程中";
        auto res = HttpResponse::newHttpJsonResponse(err);
        res->setStatusCode(drogon::k500InternalServerError);
        callback(res);
    }
}
