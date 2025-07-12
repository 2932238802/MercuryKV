#pragma once

#include <cstdint>
#include <drogon/utils/coroutine.h>
namespace Service
{

struct AlterPersonInfoReturn
{
    std::string message;
};

class AlterPersonInfo
{
  public:
    drogon::Task<AlterPersonInfoReturn> AlterInfo(const std::string &username,
                                                  const std::string &password,
                                                  const std::string &email, int64_t user_id);
};

} // namespace Service