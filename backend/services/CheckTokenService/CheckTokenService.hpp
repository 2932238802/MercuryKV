#pragma once

#include "MyJWT.hpp"
#include "MyLog.hpp"
#include "Users/Users.h"
#include "type.hpp"
#include <cstddef>
#include <cstdint>
#include <drogon/HttpResponse.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>
#include <memory>
#include <string>

namespace Service
{
using namespace common;

struct CheckTokenReturn
{
    int64_t user_id;
    size_t code;
    std::string message;
    std::string username;
};
class CheckTokenServiceFactory;
class CheckTokenService
{
  public:
    using ptr = std::shared_ptr<CheckTokenService>;
    friend class CheckTokenServiceFactory;
    drogon::Task<CheckTokenReturn> Check(std::string &&token);
    drogon::orm::DbClientPtr db_client;

  protected:
    CheckTokenService(drogon::orm::DbClientPtr db_out) : db_client(db_out)
    {
    }
};

// ----- ----- ----- ----- -----
// 工厂设计
class CheckTokenServiceFactory
{
  public:
    static CheckTokenService::ptr MakeService(drogon::orm::DbClientPtr db_client)
    {
        struct Enable : public CheckTokenService
        {
            Enable(drogon::orm::DbClientPtr db_out) : CheckTokenService(db_out)
            {
            }
        };
        return std::make_shared<Enable>(db_client);
    }
};

} // namespace Service