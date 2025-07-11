#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

class EmailVerify : public drogon::HttpController<EmailVerify>
{
  public:
    METHOD_LIST_BEGIN
    // 验证 post 请求
    // TODO: /EmailVerify/verify
    METHOD_ADD(EmailVerify::Verify, "/verify", drogon::Post);

    // 请求发送验证码 就是 get请求
    // TODO: /EmailVerify/sendemail
    METHOD_ADD(EmailVerify::SendEmail, "/sendemail/{1}", drogon::Get);

    METHOD_LIST_END
    void Verify(const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void SendEmail(const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                   std::string email_address) const;
};
