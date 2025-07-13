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

struct AlterPersonInfoReturn
{
    std::string message;
    std::int64_t code;
    std::string username;
};

class AlterPersonInfo
{
  public:
    using ptr = std::shared_ptr<AlterPersonInfo>;
    drogon::Task<AlterPersonInfoReturn> AlterInfo(const std::string &username,
                                                  const std::string &password,
                                                  const std::string &email, int64_t user_id);
};

class AlterPersonInfoFactory
{
  public:
    static AlterPersonInfo::ptr MakeService()
    {
        return std::make_shared<AlterPersonInfo>();
    }

  private:
};
} // namespace Service
