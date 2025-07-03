#include "Alter.h"
#include "KVStore/KvStore.h"
#include "KV_Tag/KvTagAssociation.h"
#include "MyLog.hpp"
#include "Tags/Tags.h"
#include <cstddef>
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
 * @brief
    1. post 请求
    2. 实现一下 增加数据的函数 插入数据库
    3. 请求发来的内容 应该有
    4. 先取出之前的内容 作为上一次的值
    5. 前端需要的内容 {user_id , key_input , value_input ,tag_name}
 *
 * @param req
 * @param callback
 */
void Alter::AddData(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback)
{

    auto jsonobject = req->getJsonObject();

    // -----------------------------------
    // 检查请求体是否存在以及字段是否完整
    if (!jsonobject)
    {
        Json::Value error;
        error["message"] = "there is a problem with your request";
        auto res = HttpResponse::newHttpJsonResponse(error);
        callback(res);
        return;
    }
    if (!(*jsonobject).isMember("user_id") || !(*jsonobject)["user_id"].isInt64() ||
        !(*jsonobject).isMember("key_input") || !(*jsonobject)["key_input"].isString() ||
        !(*jsonobject).isMember("value_input") || !(*jsonobject)["value_input"].isString() ||
        !(*jsonobject).isMember("tags") || !(*jsonobject)["tags"].isArray())
    {
        Json::Value error;
        error["error"] = "Missing or invalid field in request body";
        auto res = HttpResponse::newHttpJsonResponse(error);

        // TODO: 这里的错误码需要和前端
        res->setStatusCode(k400BadRequest);
        callback(res);
        return;
    }
    // -----------------------------------

    // -----------------------------------
    // 获取请求中的数据
    // 前端传来的数据
    // 生成一个 数据库事务
    // 前端发来的这些 信息不能为空 不然数据库插入失败
    uint64_t user_id = (*jsonobject)["user_id"].asInt64();
    std::string key_input = (*jsonobject)["key_input"].asString();
    std::string value_input = (*jsonobject)["value_input"].asString();
    const Json::Value &tags_json = (*jsonobject)["tags"];
    if (key_input == "" || value_input == "")
    {
        MY_LOG_ERROR("key_input value_input tag_name 内容为空");
        Json::Value error;
        error["message"] = "前端的key或者value或者tagname内容为空,无法插入!";
        auto res = HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(k400BadRequest);
        callback(res);
        return;
    }

    auto db_client = app().getDbClient();
    auto trans = db_client->newTransaction();

    // -----------------------------------

    // -----------------------------------
    // 插入对应的表
    try
    {
        Mapper<KvStore> mapper_kv(trans);
        Mapper<Tags> mapper_tags(trans);
        Mapper<KvTagAssociation> mapper_kta(trans);
        Tags tag;
        KvStore kv;
        KvTagAssociation kta;

        // auto old_kv_opt = mapper_kv.findBy(
        //     Criteria(KvStore::Cols::_user_id, CompareOperator::EQ, user_id) &&
        //     Criteria(KvStore::Cols::_key_input, CompareOperator::EQ, key_input));
        // if (old_kv_opt.empty()) {
        //   MY_LOG_ERROR("No previous kv found for user_id:", user_id,
        //                " and key_input:", key_input);
        // } else {
        //   std::string previous_value = old_kv_opt[0].getValueOfValueInput();
        //   kv.setPreviousValue(previous_value);
        //   MY_LOG_SUC("Previous value found for user_id:", user_id,
        //              " and key_input:", key_input, " value:", previous_value);
        // }

        // mapper_kv 插入数据
        kv.setUserId(user_id);
        kv.setKeyInput(key_input);
        kv.setValueInput(value_input);
        kv.setUpdatedAt(trantor::Date::now());
        mapper_kv.insert(kv);
        MY_LOG_SUC("插入成功!");
        int64_t kv_id = kv.getValueOfKvId();

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
                // Tag already exists, use its ID
                tag_id = existing_tags[0].getValueOfTagId();
            }
            else
            {
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

        // TODO: 前端看一眼 需要返回的内容
        Json::Value success_resp;
        Json::Value data;
        success_resp["code"] = 201;
        data["kv_id"] = kv_id;
        data["tags"] = tags_json;
        data["updated_at"] = trantor::Date::now().toDbString();
        success_resp["data"] = data;
        auto res = HttpResponse::newHttpJsonResponse(success_resp);
        res->setStatusCode(drogon::k201Created);
        callback(res);
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        trans->rollback();
        MY_LOG_ERROR("Database insertion failed: ", e.base().what());

        Json::Value error;
        error["message"] = "Database insertion failed: " + std::string(e.base().what());
        auto res = HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(drogon::k500InternalServerError);

        // 记录错误日志
        callback(res);
        return;
    }

    // 记录正确的约为
    MY_LOG_SUC("KvTagAssociation inserted successfully for user_id:", user_id);
}

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

/**
 * @brief 删除数据
 *
 * @param req
 * @param callback
 * @param kv_id
 */
void Alter::DeleteData(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback,
                       const std::string &kv_str)
{
    int64_t kv_id;
    try
    {
        kv_id = std::stoll(kv_str);
    }
    catch (const std::exception &e)
    {
        Json::Value err;
        err["message"] = "客户端kv存储的id有误 客户端错误";
        auto res = HttpResponse::newHttpJsonResponse(err);
        callback(res);
        return;
    }

    auto db_client = app().getDbClient();
    auto trans = db_client->newTransaction();

    try
    {
        // 这里是删除数据
        Mapper<KvStore> mapper(trans);
        Mapper<KvTagAssociation> mapper_kta(trans);

        mapper_kta.deleteBy(Criteria(KvTagAssociation::Cols::_kv_id, CompareOperator::EQ, kv_id));

        std::optional<KvStore> findone =
            mapper.findOne(Criteria(KvStore::Cols::_kv_id, CompareOperator::EQ, kv_id));

        if (!findone)
        {
            // 日志输出
            MY_LOG_ERROR("Data not found for kv_id: ", kv_id);
            Json::Value error;
            error["message"] = "Data not found for kv_id: " + kv_str;
            auto res = HttpResponse::newHttpJsonResponse(error);
            res->setStatusCode(k404NotFound);
            callback(res);
            return;
        }

        // 删除数据库里面数据
        size_t number = mapper.deleteByPrimaryKey(findone->getValueOfKvId());
        if (number != 0)
        {
            MY_LOG_SUC("Data deleted successfully for kv_id: ", kv_id);
        }
        else
        {
            MY_LOG_WARN("找不到对应的 kv_id 删除动作无效");

            Json::Value error;
            error["message"] = "找不到对应的 kv_id 删除动作无效: " + kv_str;
            auto res = HttpResponse::newHttpJsonResponse(error);

            // TODO: 前端看一下这里 404
            res->setStatusCode(k404NotFound);
            callback(res);
            return;
        }

        // TODO: 删除了也要返回信息
        Json::Value success_msg;
        success_msg["message"] = "Data deleted successfully";
        auto res = HttpResponse::newHttpJsonResponse(success_msg);
        res->setStatusCode(k200OK);
        callback(res);
    }

    catch (const drogon::orm::DrogonDbException &e)
    {
        Json::Value error;
        error["message"] = "Database deletion failed: " + std::string(e.base().what());
        auto res = HttpResponse::newHttpJsonResponse(error);
        res->setStatusCode(k500InternalServerError);
        callback(res);
        return;
    }
}
