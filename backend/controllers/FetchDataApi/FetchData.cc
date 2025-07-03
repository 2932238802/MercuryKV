#include "FetchData.h"
#include "KVStore/KvStore.h"
#include "KV_Tag/KvTagAssociation.h"
#include "MyLog.hpp"
#include "Tags/Tags.h"
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/SqlBinder.h>

/**
 * @brief
 * 1. 返回查询到的数据 连表查询 三个表
 * 2. 返回查询的数据即可
 *
 * @param req
 * @param callback 400
 */
using namespace drogon_model::mercury;
using namespace MyLogNS;
namespace
{
using CallbackType = std::function<void(const HttpResponsePtr &)>;
}

void FetchData::Fetch(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::string userid_str = req->getParameter("user_id");

    if (userid_str.empty())
    {
        MY_LOG_ERROR("user_id is none");

        Json::Value err;
        err["error"] = "user_id is required";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    int64_t userid;
    try
    {
        userid = std::stoll(userid_str);
    }
    catch (const std::exception &e)
    {
        MY_LOG_ERROR("user_id trans error");

        Json::Value err;
        err["error"] = "Invalid user_id format";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(drogon::k400BadRequest);
        callback(resp);
        return;
    }

    auto dbClient = drogon::app().getDbClient();
    drogon::orm::Mapper<KvStore> kvMapper(dbClient);

    // 第一次查询：获取该用户的所有 KV 记录 (使用异步)
    kvMapper.orderBy(KvStore::Cols::_updated_at, drogon::orm::SortOrder::DESC)
        .findBy(
            drogon::orm::Criteria(KvStore::Cols::_user_id, drogon::orm::CompareOperator::EQ,
                                  userid),

            // ————————————————————————————————————————————————————————————————————————
            // 第一次查询的成功回调
            [callback, dbClient](std::vector<KvStore> kvList)
            {
                // 如果用户没有任何KV记录，直接返回一个空的JSON数组
                if (kvList.empty())
                {
                    MY_LOG_WARN("没有任何标签 内容为空");

                    auto resp = HttpResponse::newHttpJsonResponse(Json::Value(Json::arrayValue));
                    callback(resp);
                    return;
                }

                // 准备第二次查询：收集所有需要查询标签的 kv_id
                std::vector<int64_t> kvIds;
                for (const auto &kv : kvList)
                {
                    kvIds.push_back(kv.getValueOfKvId());
                }
                std::string kvidsstring = "{";
                for (size_t i = 0; i < kvIds.size(); ++i)
                {
                    kvidsstring += std::to_string(kvIds[i]);
                    if (i < kvIds.size() - 1)
                    {
                        kvidsstring += ",";
                    }
                }
                kvidsstring += "}";

                // \$1 占位符
                std::string sql = "SELECT kta.kv_id, t.tag_name FROM kv_tag_association kta "
                                  "JOIN tags t ON kta.tag_id = t.tag_id "
                                  "WHERE kta.kv_id = ANY(\$1::bigint[])";

                // 使用异步 execSqlAsync
                dbClient->execSqlAsync(
                    sql,
                    [callback, kvList = std::move(kvList)](const drogon::orm::Result &result)
                    {
                        // 创建一个 id 和 名称映射的表
                        std::map<int64_t, std::vector<std::string>> kvidtotagmap;
                        for (const auto &row : result)
                        {
                            auto kvId = row["kv_id"].as<int64_t>();
                            auto tagName = row["tag_name"].as<std::string>();
                            kvidtotagmap[kvId].push_back(tagName);
                        }

                        Json::Value jsonarray(Json::arrayValue);
                        for (const auto &kv : kvList)
                        {
                            Json::Value jsonObj;
                            jsonObj["kv_id"] = (Json::Int64)kv.getValueOfKvId();
                            jsonObj["key_input"] = kv.getValueOfKeyInput();
                            jsonObj["value_input"] = kv.getValueOfValueInput();

                            // 从 map 中查找这个 kv 的 tags
                            Json::Value tagsarray(Json::arrayValue);
                            auto it = kvidtotagmap.find(kv.getValueOfKvId());

                            // 如果找到了标签
                            if (it != kvidtotagmap.end())
                            {
                                for (const auto &tag : it->second)
                                {
                                    tagsarray.append(tag);
                                }
                            }
                            jsonObj["tags"] = tagsarray;

                            jsonObj["updated_at"] = kv.getValueOfUpdatedAt().toDbString();

                            jsonarray.append(jsonObj);
                        }

                        Json::Value finalResponse;
                        finalResponse["code"] = 200;
                        finalResponse["data"] = jsonarray;
                        auto resp = HttpResponse::newHttpJsonResponse(finalResponse);
                        MY_LOG_SUC("数据读取成功!", finalResponse);
                        callback(resp);
                    },

                    [callback](const drogon::orm::DrogonDbException &e)
                    {
                        Json::Value err;
                        //   TODO: message
                        err["message"] = e.base().what();
                        auto resp = HttpResponse::newHttpJsonResponse(err);

                        //   TODO: 设置500
                        resp->setStatusCode(drogon::k500InternalServerError);
                        callback(resp);
                    },
                    kvidsstring);
            },

            // ————————————————————————————————————————————————————————————————————————
            [callback](const drogon::orm::DrogonDbException &e)
            {
                MY_LOG_ERROR("后端抓取数据错误");

                Json::Value err;
                // TODO: 前端看一眼
                //  500 message
                err["message"] = e.base().what();
                auto resp = HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(drogon::k500InternalServerError);
                callback(resp);
            });
}