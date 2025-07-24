#pragma once
#include "KVStore/KvStore.h"
#include "KV_Tag/KvTagAssociation.h"
#include "MyLog.hpp"
#include "Tags/Tags.h"
#include "Type.hpp"
#include <cstddef>
#include <cstdint>
#include <drogon/HttpResponse.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Exception.h>
#include <drogon/utils/coroutine.h>
#include <exception>
#include <json/value.h>
#include <memory>
#include <string>
#include <sys/types.h>
#include <trantor/utils/Date.h>

namespace Service
{
using namespace common;

enum class AlterType
{
    AlterData = 0,
    AlterPassword = 1
};

class AddDataServiceFactory;

struct AlterDataReturn
{
    int64_t code;
    int64_t kv_id;
    std::string key_input;
    std::string value_input;
    std::string updated_at;
};

struct AlterPasswordReturn
{
    std::string message;
    size_t code;
};

class BaseService
{
  public:
    using ptr = std::shared_ptr<BaseService>;
    virtual ~BaseService() = default;
};
template <class R> class AlterService : public BaseService
{
  public:
    drogon::orm::DbClientPtr db_client;
    using ptr = std::shared_ptr<AlterService<R>>;
    virtual drogon::Task<R> Alter(const Json::Value &sjon) = 0;

  protected:
    AlterService(drogon::orm::DbClientPtr db_out) : db_client(db_out)
    {
    }
};
class AlterDataService : public AlterService<Service::AlterDataReturn>
{
  public:
    drogon::Task<AlterDataReturn> Alter(const Json::Value &sjon) override;
    AlterDataService(drogon::orm::DbClientPtr db_out)
        : AlterService<Service::AlterDataReturn>(db_out)
    {
    }
};
class AlterPasswordService : public AlterService<Service::AlterPasswordReturn>
{
  public:
    drogon::Task<AlterPasswordReturn> Alter(const Json::Value &sjon) override;
    AlterPasswordService(drogon::orm::DbClientPtr db_out)
        : AlterService<Service::AlterPasswordReturn>(db_out)
    {
    }
};
class AlterDataServiceFactory
{
  public:
    static Service::BaseService::ptr MakeService(AlterType type, drogon::orm::DbClientPtr db_out)
    {
        if (type == AlterType::AlterData)
        {
            return std::make_shared<Service::AlterDataService>(db_out);
        }
        else if (type == AlterType::AlterPassword)
        {
            return std::make_shared<Service::AlterPasswordService>(db_out);
        }
        return nullptr;
    }
};
} // namespace Service
