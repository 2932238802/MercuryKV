#include "KvController.h"
// 引入 ORM Mapper 和 JsonCpp
#include <drogon/orm/Mapper.h>
#include <json/json.h>
// 引入你的数据模型类
#include "models/KvStore.h"
#include "models/KvTagAssociation.h"
#include "models/Tags.h"

#include <map>

using namespace drogon_model::postgres; // 假设你使用PostgreSQL

void KvController::getUserKvPairs(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  // 1. 获取用户ID
  // 假设您的认证中间件(Filter)已经验证了JWT，并将user_id存入了请求属性
  auto userId = req->getAttribute<int64_t>("user_id");
  if (!userId) {
    // 如果没有用户ID，说明未授权
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k401Unauthorized);
    callback(resp);
    return;
  }

  auto dbClient = drogon::app().getDbClient();
  drogon::orm::Mapper<KvStore> kvMapper(dbClient);

  // 2. 第一次查询：获取该用户的所有KV记录
  // 使用Drogon ORM，按更新时间倒序排列
  kvMapper.orderBy(KvStore::Cols::_updated_at, drogon::orm::SortOrder::DESC)
      .findBy(
          drogon::orm::Criteria(KvStore::Cols::_user_id,
                                drogon::orm::CompareOp::EQ, userId),
          // 第一次查询的成功回调
          [callback, dbClient](std::vector<KvStore> kvList) {
            // 如果用户没有任何KV记录，直接返回一个空的JSON数组，流程结束
            if (kvList.empty()) {
              auto resp = HttpResponse::newHttpJsonResponse(
                  Json::Value(Json::arrayValue));
              callback(resp);
              return;
            }

            // 3. 准备第二次查询：收集所有需要查询标签的 kv_id
            std::vector<int64_t> kvIds;
            for (const auto &kv : kvList) {
              kvIds.push_back(kv.getValueOfKvId());
            }

            // 4. 第二次查询：使用原生SQL一次性获取所有相关标签
            // 使用 `ANY(\$1::bigint[])` 是PostgreSQL中高效查询IN (...)的方式
            std::string sql =
                "SELECT kta.kv_id, t.tag_name FROM kv_tag_association kta "
                "JOIN tags t ON kta.tag_id = t.tag_id "
                "WHERE kta.kv_id = ANY(\$1::bigint[])";

            dbClient->execSqlAsync(
                sql,
                // 第二次查询的成功回调
                [callback, kvList = std::move(kvList)](
                    const drogon::orm::Result &result) {
                  // 5. 在内存中构建一个高效的查找表 (map)
                  // 键是 kv_id，值是该kv_id对应的所有标签名
                  std::map<int64_t, std::vector<std::string>> kvIdToTagsMap;
                  for (auto const &row : result) {
                    auto kvId = row["kv_id"].as<int64_t>();
                    auto tagName = row["tag_name"].as<std::string>();
                    kvIdToTagsMap[kvId].push_back(tagName);
                  }

                  // 6. 组装最终的JSON响应
                  // 遍历第一次查询到的kvList，并使用map来查找它们的标签
                  Json::Value jsonArray(Json::arrayValue);
                  for (const auto &kv : kvList) {
                    Json::Value jsonObj;
                    jsonObj["id"] = (Json::Int64)kv.getValueOfKvId();
                    jsonObj["key"] = kv.getValueOfKeyInput();

                    // Drogon对JSONB支持很好，可以直接赋值给Json::Value
                    jsonObj["value"] = kv.getValueOfValueInput();

                    // 从map中查找这个kv的tags
                    Json::Value tagsArray(Json::arrayValue);
                    auto it = kvIdToTagsMap.find(kv.getValueOfKvId());
                    if (it != kvIdToTagsMap.end()) { // 如果找到了
                      for (const auto &tag : it->second) {
                        tagsArray.append(tag);
                      }
                    }
                    jsonObj["tags"] = tagsArray;

                    // 格式化时间戳为 "YYYY-MM-DD HH:MM:SS" 格式
                    jsonObj["updated_at"] =
                        kv.getValueOfUpdatedAt().toDbString(false);

                    jsonArray.append(jsonObj);
                  }

                  auto resp = HttpResponse::newHttpJsonResponse(jsonArray);
                  callback(resp);
                },
                // 第二次查询的异常处理
                [callback](const drogon::orm::DrogonDbException &e) {
                  Json::Value err;
                  err["error"] = "Database error while fetching tags.";
                  err["detail"] = e.base().what();
                  auto resp = HttpResponse::newHttpJsonResponse(
                      err, k500InternalServerError);
                  callback(resp);
                },
                kvIds); // 将kvIds作为参数绑定到SQL的\$1
          },
          // 第一次查询的异常处理
          [callback](const drogon::orm::DrogonDbException &e) {
            Json::Value err;
            err["error"] = "Database error while fetching kv pairs.";
            err["detail"] = e.base().what();
            auto resp =
                HttpResponse::newHttpJsonResponse(err, k500InternalServerError);
            callback(resp);
          });
}
