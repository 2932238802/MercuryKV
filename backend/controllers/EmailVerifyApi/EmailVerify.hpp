#pragma once

#include "AlterPersonInfoService/AlterPersonInfo.hpp"
#include "EmailService/EmailService.hpp"
#include "MyLog.hpp"
#include "RandStr.hpp"
#include "type.hpp"
#include <cstdint>
#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include <drogon/nosql/RedisClient.h>
#include <drogon/nosql/RedisException.h>
#include <drogon/orm/Exception.h>
#include <exception>
#include <json/value.h>
#include <string>

using namespace common;

/**
 * @brief 验证码的验证 以及 完整修改密码和账号 邮箱
 *
 */
class EmailVerify : public drogon::HttpController<EmailVerify>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(EmailVerify::VerifyAndModify, "/verifyandmodify", drogon::Post);
    METHOD_ADD(EmailVerify::SendEmail, "/sendemail/{1}", drogon::Get);
    METHOD_LIST_END
    drogon::Task<drogon::HttpResponsePtr> VerifyAndModify(drogon::HttpRequestPtr req);
    drogon::Task<drogon::HttpResponsePtr> SendEmail(drogon::HttpRequestPtr req,
                                                    const std::string &email_address);
};
