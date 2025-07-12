#pragma once

#include "EmailService/EmailService.hpp"
#include "MyLog.hpp"
#include "RandStr.hpp"
#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include <drogon/nosql/RedisClient.h>
#include <drogon/nosql/RedisException.h>
#include <exception>
#include <json/value.h>

using namespace common;

class EmailVerify : public drogon::HttpController<EmailVerify>
{
  public:
    METHOD_LIST_BEGIN
    // 验证 post 请求
    // TODO: /EmailVerify/verify
    METHOD_ADD(EmailVerify::VerifyAndModify, "/verify/{1}/{2}", drogon::Get);

    // 请求发送验证码 就是 get请求
    // TODO: /EmailVerify/sendemail
    METHOD_ADD(EmailVerify::SendEmail, "/sendemail/{1}", drogon::Get);

    METHOD_LIST_END
    drogon::Task<drogon::HttpResponsePtr> VerifyAndModify(const drogon::HttpRequestPtr &req,
                                                          const std::string &user_code,
                                                          const std::string &email);

    drogon::Task<drogon::HttpResponsePtr> SendEmail(const drogon::HttpRequestPtr &req,
                                                    std::string email_address) const;
};
