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
                    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto jsonobject = req->getJsonObject();

  // -----------------------------------
  // 检查请求体是否存在以及字段是否完整
  if (!jsonobject) {
    Json::Value error;
    error["message"] = "there is a problem with your request";
    auto res = HttpResponse::newHttpJsonResponse(error);
    callback(res);
    return;
  }
  if (!(*jsonobject).isMember("user_id") ||
      !(*jsonobject)["user_id"].isInt64() ||
      !(*jsonobject).isMember("key_input") ||
      !(*jsonobject)["key_input"].isString() ||
      !(*jsonobject).isMember("value_input") ||
      !(*jsonobject)["value_input"].isString() ||
      !(*jsonobject).isMember("tag_name") ||
      !(*jsonobject)["tag_name"].isString()) {
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
  uint64_t user_id = (*jsonobject)["user_id"].asInt64();
  std::string key_input = (*jsonobject)["key_input"].asString();
  std::string value_input = (*jsonobject)["value_input"].asString();
  std::string tag_name = (*jsonobject)["tag_name"].asString();
  auto db_client = app().getDbClient();
  auto trans = db_client->newTransaction();
  // -----------------------------------

  // -----------------------------------
  // 插入对应的表
  try {
    Mapper<KvStore> mapper_kv(trans);
    Mapper<Tags> mapper_tags(trans);
    Mapper<KvTagAssociation> mapper_kta(trans);
    Tags tag;
    KvStore kv;
    KvTagAssociation kta;
    std::string previous_value = "";
    auto old_kv_opt = mapper_kv.findBy(
        Criteria(KvStore::Cols::_user_id, CompareOperator::EQ, user_id) &&
        Criteria(KvStore::Cols::_key_input, CompareOperator::EQ, key_input));

    // 如果找到了则获取之前的值
    // 如果没有找到对应的 kv 则创建一个新的
    if (old_kv_opt.empty()) {
      MY_LOG_SUC("No previous kv found for user_id:", user_id,
                 " and key_input:", key_input);
    } else {
      previous_value = old_kv_opt[0].getValueOfValueInput();
      MY_LOG_SUC("Previous value found for user_id:", user_id,
                 " and key_input:", key_input, " value:", previous_value);
    }

    // mapper_kv 插入数据
    kv.setUserId(user_id);
    kv.setKeyInput(key_input);
    kv.setValueInput(value_input);
    kv.setPreviousValue(previous_value);
    kv.setUpdatedAt(trantor::Date::now());
    mapper_kv.insert(kv);

    // Tags 插入数据
    int64_t kv_id = kv.getValueOfKvId();
    int64_t tag_id;
    auto existing_tag_opt = mapper_tags.findBy(
        Criteria(Tags::Cols::_user_id, CompareOperator::EQ, user_id) &&
        Criteria(Tags::Cols::_tag_name, CompareOperator::EQ, tag_name));
    if (!existing_tag_opt.empty()) {
      tag_id = existing_tag_opt[0].getValueOfTagId();
      MY_LOG_INF("Tag '", tag_name, "' already exists with id: ", tag_id);
    } else {
      Tags new_tag;
      new_tag.setUserId(user_id);
      new_tag.setTagName(tag_name);
      mapper_tags.insert(new_tag);
      tag_id = new_tag.getValueOfTagId(); // 正确：插入后获取 ID
      MY_LOG_SUC("New tag '", tag_name, "' inserted with id: ", tag_id);
    }

    // KvTagAssociation 插入数据
    kta.setKvId(kv_id);
    kta.setTagId(tag_id);
    mapper_kta.insert(kta);

    Json::Value success_resp;
    Json::Value data;

    // TODO: 前端看一眼 需要返回的内容
    success_resp["code"] = 201;
    success_resp["message"] = "Data and tag associated successfully";
    data["kv_id"] = kv_id;
    data["tag_id"] = tag_id;
    data["updated_at"] = trantor::Date::now().toDbString();
    success_resp["data"] = data;
    auto res = HttpResponse::newHttpJsonResponse(success_resp);
    res->setStatusCode(drogon::k201Created);
    callback(res);
  } catch (const drogon::orm::DrogonDbException &e) {
    Json::Value error;
    error["message"] =
        "Database insertion failed: " + std::string(e.base().what());
    auto res = HttpResponse::newHttpJsonResponse(error);
    res->setStatusCode(drogon::k500InternalServerError);

    // 记录错误日志
    MY_LOG_ERROR("Database insertion failed: ", e.base().what());
    callback(res);
    return;
  }
  // -----------------------------------

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
                      std::function<void(const HttpResponsePtr &)> &&callback) {

  // 获取请求体中的 JSON 对象
  auto jsonobject = req->getJsonObject();
  if (!jsonobject) {
    Json::Value error;
    error["message"] = "Invalid kv_id or Invalid request body";
    auto res = HttpResponse::newHttpJsonResponse(error);
    res->setStatusCode(k400BadRequest);
    callback(res);
    return;
  }

  // 不为空
  // 不相信前端原则
  if (!(*jsonobject).isMember("user_id") ||
      !(*jsonobject)["user_id"].isInt64() || !(*jsonobject).isMember("kv_id") ||
      !(*jsonobject)["kv_id"].isInt64() ||
      !(*jsonobject).isMember("key_input") ||
      !(*jsonobject)["key_input"].isString() ||
      !(*jsonobject).isMember("value_input") ||
      !(*jsonobject)["value_input"].isString() ||
      !(*jsonobject).isMember("tag_name") ||
      !(*jsonobject)["tag_name"].isString()) {
    Json::Value error;
    error["error"] = "Missing or invalid field in request body";
    auto res = HttpResponse::newHttpJsonResponse(error);
    res->setStatusCode(k400BadRequest);
    callback(res);
    return;
  }

  // 数据库生成
  // 生成事务
  auto db_client = app().getDbClient();
  auto trans = db_client->newTransaction();

  // 修改数据
  // 先根据主键 找到这个数据 然后 对C++ 进行修改 然后 mapper.update
  // 进行修改到数据库
  // TODO: 改一下
  try {
    Mapper<KvStore> mapper(trans);
    Mapper<Tags> mapper_tags(trans);

    // 传到这里 说明了 被摆放在该带对应的数据
    // 先前的数据
    int64_t kv_id = (*jsonobject)["kv_id"].asInt64();
    auto key_input = (*jsonobject)["key_input"].asString();
    auto value_input = (*jsonobject)["value_input"].asString();
    auto updated_at = trantor::Date::now();
    auto tag_name = (*jsonobject)["tag_name"].asString();

    std::optional<KvStore> kv_store = mapper.findOne(
        Criteria(KvStore::Cols::_kv_id, CompareOperator::EQ, kv_id));

    // 获取之前的数据
    auto value_previous = kv_store->getValueOfKeyInput();

    if (kv_store) {
      Json::Value error;
      error["message"] = "user_id is wrong or not found";
      MY_LOG_ERROR("user_id is wrong or not found");
      auto res = HttpResponse::newHttpJsonResponse(error);
      res->setStatusCode(k404NotFound);
      callback(res);
      return;
    }

    // 更新数据
    kv_store->setKeyInput(key_input);
    kv_store->setValueInput(value_input);
    kv_store->setUpdatedAt(updated_at);
    kv_store->setPreviousValue(value_previous);
    kv_store->setUpdatedAt(updated_at);

    // 更新数据库
    mapper.update(*kv_store);

    // 更新另一个表 Tags
    auto tag_id = kv_store->getValueOfKvId();
    std::optional<Tags> existing_tag = mapper_tags.findOne(
        Criteria(Tags::Cols::_user_id, CompareOperator::EQ, tag_id) &&
        Criteria(Tags::Cols::_tag_name, CompareOperator::EQ, tag_name));

    if (existing_tag) {
      MY_LOG_SUC("New tag '", existing_tag->getTagName());

      Tags new_tag;
      existing_tag->setTagName(tag_name);
      mapper_tags.insert(*existing_tag);
    } else {
      MY_LOG_ERROR("Tag not found");

      Json::Value error;
      error["message"] = "Tag not found";
      auto res = HttpResponse::newHttpJsonResponse(error);
      res->setStatusCode(k404NotFound);
      callback(res);
      return;
    }
  } catch (const drogon::orm::DrogonDbException &e) {
    Json::Value error;
    error["message"] =
        "Database insertion failed: " + std::string(e.base().what());
    auto res = HttpResponse::newHttpJsonResponse(error);
    res->setStatusCode(k500InternalServerError);
    callback(res);
    return;
  }
}

void Alter::DeleteData(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback,
                       const std::string &kv_id) {

  auto jsonobject = req->getJsonObject();
  if (!jsonobject) {
    Json::Value error;
    error["message"] = "Invalid request body";
    auto res = HttpResponse::newHttpJsonResponse(error);
    res->setStatusCode(k400BadRequest);
    callback(res);
    return;
  }

  // 删除 名为kv_id
  auto db_client = app().getDbClient();
  auto trans = db_client->newTransaction();

  try {
    // 这里是删除数据
    Mapper<KvStore> mapper(trans);

    std::optional<KvStore> findone = mapper.findOne(
        Criteria(KvStore::Cols::_kv_id, CompareOperator::EQ, kv_id));

    if (!findone) {
      // 日志输出
      MY_LOG_ERROR("Data not found for kv_id: ", kv_id);

      Json::Value error;
      error["message"] = "Data not found for kv_id: " + kv_id;
      auto res = HttpResponse::newHttpJsonResponse(error);
      res->setStatusCode(k404NotFound);
      callback(res);
      return;
    }

    // 删除数据库里面数据
    mapper.deleteByPrimaryKey(findone->getValueOfKvId());

    MY_LOG_SUC("Data deleted successfully for kv_id: ", kv_id);
  }

  catch (const drogon::orm::DrogonDbException &e) {
    Json::Value error;
    error["message"] =
        "Database deletion failed: " + std::string(e.base().what());
    auto res = HttpResponse::newHttpJsonResponse(error);
    res->setStatusCode(k500InternalServerError);
    callback(res);
    return;
  }
}
