
#pragma once
#include "MyLog.hpp"
#include "Poco/Net/MailMessage.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/SecureSMTPClientSession.h"
#include "Poco/Poco.h"
#include "SmptPlugin/SmptPlugin.hpp"
#include "type.hpp"
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <cstdint>
#include <drogon/HttpAppFramework.h>
#include <drogon/utils/coroutine.h>
#include <exception>

namespace Service
{
struct EmailServiceReturn
{
    int64_t code;
    std::string message;
};

class EmailService
{
  public:
    static void SendEmail(const std::string &to, const std::string &code);
    static drogon::Task<Service::EmailServiceReturn> Verify(std::string email_address,
                                                            std::string user_code);

  private:
    static std::string BuildeEmailBodyForModifyPersonInfo(const std::string &code);
};

} // namespace Service