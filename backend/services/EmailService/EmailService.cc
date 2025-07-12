
#include "EmailService.hpp"
#include "MyLog.hpp"
#include "type.hpp"
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpResponse.h>
#include <drogon/utils/coroutine.h>
#include <exception>
#include <functional>
#include <future>
#include <json/value.h>

std::string Service::EmailService::BuildeEmailBodyForModifyPersonInfo(const std::string &code)
{
    return R"(
        <!DOCTYPE html>
        <html>
        <head>
            <style>
                body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif; }
                .container { max-width: 600px; margin: auto; padding: 20px; border: 1px solid #eee; border-radius: 5px; }
                .code { font-size: 32px; font-weight: 600; color: #007BFF; letter-spacing: 2px; }
            </style>
        </head>
        <body>
            <div class="container">
                <h2>MercuryKV</h2>
                <p>hello hello 我是来自Mercury的消息验证 下面是你修改个人信息的验证码</p>
                <p class="code">)" +
           code + R"(</p>
                <p>This code is valid for 10 minutes.</p>
            </div>
        </body>
        </html>
    )";
}

/**
 * @brief 无状态发送邮件
 *
 * @param to 发送给谁
 * @param code 发送什么信息
 * @return drogon::Task<>
 */
void Service::EmailService::SendEmail(const std::string &to, const std::string &code)
{
    // to 发送给谁
    // code 验证码

    std::thread(
        [=]()
        {
            auto smtp = drogon::app().getPlugin<Plugin::SmptPlugin>();

            if (!smtp)
            {
                MY_LOG_ERROR("stmp 内部服务错误");
            }

            // 说明服务正常解析信息
            const auto &smtp_config = smtp->getConfig();
            Poco::Net::MailMessage message;

            std::string sender{smtp_config.from_address}, recipient{to}, subject{"Mercury 验证码"},
                content{BuildeEmailBodyForModifyPersonInfo(code)}, auth_code{smtp_config.password};

            try
            {
                Poco::Net::MailMessage message;
                message.setSender(sender);
                message.addRecipient(Poco::Net::MailRecipient(
                    Poco::Net::MailRecipient::PRIMARY_RECIPIENT, recipient));
                message.setSubject(subject);
                message.setContent(content);
                Poco::Net::SecureSMTPClientSession session(smtp_config.host, smtp_config.port);

                session.login();

                if (session.startTLS())
                {
                    session.login(Poco::Net::SMTPClientSession::AUTH_LOGIN, smtp_config.username,
                                  smtp_config.password);
                }
                else
                {
                    MY_LOG_WARN("tls 加密失败 进入普通话");
                    session.login(Poco::Net::SMTPClientSession::AUTH_LOGIN, smtp_config.username,
                                  smtp_config.password);
                }
                session.sendMessage(message);
                session.close();
            }
            catch (const Poco::Net::NetException &e)
            {
                MY_LOG_ERROR("后台邮件线程发生Poco网络异常: {}", e.what());
            }
            catch (const std::exception &e)
            {
                MY_LOG_ERROR("后台邮件线程发生未知异常: {}", e.what());
            }
        })
        .detach();
}

drogon::Task<Service::EmailServiceReturn> Service::EmailService::Verify(std::string email_address,
                                                                        std::string user_code)
{
    std::string rediskey = "verify:code:" + email_address;
    try
    {
        auto redis_client = drogon::app().getRedisClient();
        auto trans = co_await redis_client->newTransactionCoro();

        auto result = co_await redis_client->execCommandCoro("GET %s", rediskey.c_str());
        if (result.type() == drogon::nosql::RedisResultType::kNil)
        {
            throw RequestWrong("请求内容错误");
        }

        std::string correct_code = result.asString();

        if (correct_code == user_code)
        {
            // 如果验证码 是一样的
            EmailServiceReturn ret;
            ret.code = 200;
            ret.message = "验证码成功";
            co_return ret;
        }
        else
        {
            EmailServiceReturn ret;
            ret.code = 400;
            ret.message = "验证码错误";
            co_return ret;
        }
    }
    catch (const drogon::nosql::RedisException &e)
    {
        throw RedisOperatorWrong("Redis 操作失败:", e.what());
    }
    catch (const std::exception &e)
    {
        throw UnkownWrong(e.what());
    }
}