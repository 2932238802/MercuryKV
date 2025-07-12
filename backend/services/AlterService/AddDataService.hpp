
#pragma once
#include "KVStore/KvStore.h"
#include "KV_Tag/KvTagAssociation.h"
#include "Tags/Tags.h"
#include "type.hpp"
#include <cstdint>
#include <drogon/HttpResponse.h>
#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>
#include <memory>
#include <string>
#include <sys/types.h>
#include <trantor/utils/Date.h>

namespace Service
{
using db_t = drogon::orm::DbClientPtr;
class AddDataServiceFactory;

struct AddDataReturn
{
    int64_t code;
    int64_t kv_id;
    std::string key_input;
    std::string value_input;
    std::string updated_at;
};

class AddDataService
{
  public:
    drogon::orm::DbClientPtr db_client;
    using ptr = std::shared_ptr<AddDataService>;
    virtual drogon::Task<AddDataReturn> AddData(const Json::Value &json);

  protected:
    AddDataService(db_t db_out) : db_client(db_out)
    {
    }
};

class AddDataServiceFactory
{
  public:
    static AddDataService::ptr MakeService(db_t db_out)
    {
        struct Enable : public AddDataService
        {
            Enable(db_t db_out) : AddDataService(db_out)
            {
            }
        };
        return std::make_shared<Enable>(db_out);
    }
};

} // namespace Service