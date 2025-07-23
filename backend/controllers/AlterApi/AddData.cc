#include "Alter.h"

using namespace drogon;
using namespace drogon_model::mercury;
using namespace drogon::orm;
using namespace common;

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
drogon::Task<drogon::HttpResponsePtr> Alter::AddData(HttpRequestPtr req)
{

    auto jsonobject = req->getJsonObject();
    auto db_client = app().getDbClient();
    Service::AddDataService::ptr service = Service::AddDataServiceFactory::MakeService(db_client);

    try
    {
        auto ret = co_await service->AddData(*jsonobject);
        const Json::Value &tags_json = (*jsonobject)["tags"];
        Json::Value success_resp;
        Json::Value data;
        success_resp["code"] = ret.code;
        data["key_input"] = ret.key_input;
        data["value_input"] = ret.value_input;
        data["kv_id"] = ret.kv_id;
        data["tags"] = tags_json;
        data["updated_at"] = ret.updated_at;
        success_resp["data"] = data;
        auto res = HttpResponse::newHttpJsonResponse(success_resp);
        res->setStatusCode(drogon::k201Created);
        co_return res;
    }
    catch (const common::BaseException &e)
    {
        Json::Value error;
        if (dynamic_cast<const common::DBOperatorWrong *>(&e))
        {
            error["code"] = 500;
            error["message"] = "服务器内部错误，请稍后重试";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(drogon::k500InternalServerError);
            co_return resp;
        }
        else if (dynamic_cast<const common::UnkownWrong *>(&e))
        {
            error["code"] = 501;
            error["message"] = "服务器发生未知错误";
            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(drogon::k500InternalServerError);
            co_return resp;
        }
    }

    // catch (const Service::DBOperatorWrong &)
    // {
    //     Json::Value error;
    //     error["code"] = 500;
    //     error["message"] = "服务器内部错误，请稍后重试";
    //     auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
    //     resp->setStatusCode(drogon::k500InternalServerError);
    //     co_return resp;
    // }
    // catch (const Service::UnKownWrong &e)
    // {
    //     Json::Value error;
    //     error["code"] = 500;
    //     error["message"] = "服务器发生未知错误";
    //     auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
    //     resp->setStatusCode(drogon::k500InternalServerError);
    //     co_return resp;
    // }

    // // -----------------------------------
    // // 检查请求体是否存在以及字段是否完整
    // if (!jsonobject)
    // {
    //     Json::Value error;
    //     error["message"] = "there is a problem with your request";
    //     auto res = HttpResponse::newHttpJsonResponse(error);
    //     callback(res);
    //     return;
    // }
    // if (!(*jsonobject).isMember("user_id") || !(*jsonobject)["user_id"].isInt64() ||
    //     !(*jsonobject).isMember("key_input") || !(*jsonobject)["key_input"].isString() ||
    //     !(*jsonobject).isMember("value_input") || !(*jsonobject)["value_input"].isString() ||
    //     !(*jsonobject).isMember("tags") || !(*jsonobject)["tags"].isArray())
    // {
    //     Json::Value error;
    //     error["error"] = "Missing or invalid field in request body";
    //     auto res = HttpResponse::newHttpJsonResponse(error);

    //     // TODO: 这里的错误码需要和前端
    //     res->setStatusCode(k400BadRequest);
    //     callback(res);
    //     return;
    // }
    // // -----------------------------------

    // // -----------------------------------
    // // 获取请求中的数据
    // // 前端传来的数据
    // // 生成一个 数据库事务
    // // 前端发来的这些 信息不能为空 不然数据库插入失败
    // uint64_t user_id = (*jsonobject)["user_id"].asInt64();
    // std::string key_input = (*jsonobject)["key_input"].asString();
    // std::string value_input = (*jsonobject)["value_input"].asString();
    // const Json::Value &tags_json = (*jsonobject)["tags"];

    // MY_LOG_INF("user_id:", user_id, "\nkey_input:", key_input, "\nvalue_input", value_input,
    // "/n",
    //            tags_json);

    // if (key_input == "" || value_input == "")
    // {
    //     MY_LOG_ERROR("key_input value_input tag_name 内容为空");
    //     Json::Value error;
    //     error["message"] = "前端的key或者value或者tagname内容为空,无法插入!";
    //     auto res = HttpResponse::newHttpJsonResponse(error);
    //     res->setStatusCode(k400BadRequest);
    //     callback(res);
    //     return;
    // }

    // auto db_client = app().getDbClient();
    // auto trans = db_client->newTransaction();

    // try
    // {
    //     Mapper<KvStore> mapper_kv(trans);
    //     Mapper<Tags> mapper_tags(trans);
    //     Mapper<KvTagAssociation> mapper_kta(trans);
    //     Tags tag;
    //     KvStore kv;
    //     KvTagAssociation kta;

    //     Json::Value value_input_json(value_input);
    //     Json::StreamWriterBuilder writer;
    //     std::string value_input_for_db = Json::writeString(writer, value_input_json);
    //     kv.setUserId(user_id);
    //     kv.setKeyInput(key_input);
    //     kv.setValueInput(value_input_for_db);
    //     kv.setUpdatedAt(trantor::Date::now());
    //     mapper_kv.insert(kv);
    //     int64_t kv_id = kv.getValueOfKvId();

    //     for (const auto &tag_json : tags_json)
    //     {
    //         if (!tag_json.isString())
    //             continue;
    //         std::string tag_name = tag_json.asString();
    //         if (tag_name.empty())
    //             continue;

    //         int64_t tag_id;
    //         auto existing_tags =
    //             mapper_tags.findBy(Criteria(Tags::Cols::_user_id, CompareOperator::EQ, user_id)
    //             &&
    //                                Criteria(Tags::Cols::_tag_name, CompareOperator::EQ,
    //                                tag_name));

    //         if (!existing_tags.empty())
    //         {
    //             // Tag already exists, use its ID
    //             tag_id = existing_tags[0].getValueOfTagId();
    //         }
    //         else
    //         {
    //             Tags new_tag;
    //             new_tag.setUserId(user_id);
    //             new_tag.setTagName(tag_name);
    //             mapper_tags.insert(new_tag);
    //             tag_id = new_tag.getValueOfTagId();
    //         }

    //         KvTagAssociation kta;
    //         kta.setKvId(kv_id);
    //         kta.setTagId(tag_id);
    //         mapper_kta.insert(kta);
    //     }

    //     // TODO: 前端看一眼 需要返回的内容
    //     Json::Value success_resp;
    //     Json::Value data;
    //     success_resp["code"] = 201;
    //     data["key_input"] = key_input;
    //     data["value_input"] = value_input;
    //     data["kv_id"] = kv_id;
    //     data["tags"] = tags_json;
    //     data["updated_at"] = trantor::Date::now().toDbString();
    //     success_resp["data"] = data;
    //     auto res = HttpResponse::newHttpJsonResponse(success_resp);
    //     res->setStatusCode(drogon::k201Created);
    //     callback(res);
    // }
    // catch (const drogon::orm::DrogonDbException &e)
    // {
    //     trans->rollback();
    //     MY_LOG_ERROR("Database insertion failed: ", e.base().what());

    //     Json::Value error;
    //     error["message"] = "Database insertion failed: " + std::string(e.base().what());
    //     auto res = HttpResponse::newHttpJsonResponse(error);
    //     res->setStatusCode(drogon::k500InternalServerError);

    //     // 记录错误日志
    //     callback(res);
    //     return;
    // }

    // // 记录正确的约为
    // MY_LOG_SUC("KvTagAssociation inserted successfully for user_id:", user_id);
}