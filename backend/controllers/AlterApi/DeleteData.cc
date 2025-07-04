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
        res->setStatusCode(drogon::k400BadRequest);
        callback(res);
        return;
    }

    auto db_client = app().getDbClient();
    auto trans = db_client->newTransaction();

    try
    {
        // 这里是删除数据
        Mapper<KvStore> mapper(trans);

        std::optional<KvStore> findone =
            mapper.findOne(Criteria(KvStore::Cols::_kv_id, CompareOperator::EQ, kv_id));

        if (!findone)
        {
            // 日志输出
            MY_LOG_ERROR("删除的数据不存在 ", kv_id);
            Json::Value error;
            error["message"] = "删除的数据不存在: " + kv_str;
            auto res = HttpResponse::newHttpJsonResponse(error);
            res->setStatusCode(k404NotFound);
            callback(res);
            return;
        }

        Mapper<KvTagAssociation> mapper_kta(trans);

        mapper_kta.deleteBy(Criteria(KvTagAssociation::Cols::_kv_id, CompareOperator::EQ, kv_id));

        // 删除数据库里面数据
        mapper.deleteByPrimaryKey(kv_id);

        Json::Value success_msg;
        success_msg["message"] = "数据删除成功";
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
