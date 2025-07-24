
#pragma once
#include "ConfigManage/ConfigManage.hpp"
#include "MyLog.hpp"
#include "Poco/Net/MailMessage.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/SecureSMTPClientSession.h"
#include "Poco/Net/SecureStreamSocket.h"
#include "Poco/Poco.h"
#include "SmptUtil/SmptUtil.hpp"
#include "Type.hpp"
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <cstdint>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <exception>
#include <functional>
#include <future>
#include <json/value.h>

// Poco 相关
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/SecureSMTPClientSession.h>
#include <memory>

namespace Service
{
using namespace common;
struct EmailServiceReturn
{
    int64_t code;
    std::string message;
};

class EmailService
{
  public:
    using ptr = std::shared_ptr<EmailService>;
    static void SendEmail(const std::string &to, const std::string &code);
    static drogon::Task<Service::EmailServiceReturn> Verify(std::string email_address,
                                                            std::string user_code);

  private:
    static std::string BuildeEmailBodyForModifyPersonInfo(const std::string &code);
};

class EmailServiceFactory
{

  public:
    static EmailService::ptr MakeService()
    {
        return std::make_shared<EmailService>();
    }
};

} // namespace Service