
#include "EmailService.hpp"
#include "MyLog.hpp"
static std::once_flag poco_ssl_init_flag;
void InitializePocoSSLOnce()
{
    try
    {
        Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> pCertHandler =
            new Poco::Net::AcceptCertificateHandler(false);
        const std::string caLocation = "/etc/ssl/certs/ca-certificates.crt";

        Poco::Net::Context::Ptr pcontext = new Poco::Net::Context(
            Poco::Net::Context::CLIENT_USE, "", "",
            caLocation, // caLocation
            Poco::Net::Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

        Poco::Net::SSLManager::instance().initializeClient(nullptr, pCertHandler, pcontext);
    }
    catch (const Poco::Exception &e)
    {
        throw common::UnkownWrong("Poco SSL/TLS context initialization failed: " + e.displayText());
    }
}

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

    std::call_once(poco_ssl_init_flag, InitializePocoSSLOnce);

    const auto &smtpconfig = Utils::ConfigManage::GetInstance().GetSmtpConfig();
    MY_LOG_INF(smtpconfig);
    MY_LOG_INF("to:", to);
    MY_LOG_INF("code:", code);

    Utils::SmptUtil sc(smtpconfig);

    if (!sc.IsConfigured())
    {
        throw ConfigWrong("stmp 配置错误");
    }

    // 说明服务正常解析信息
    const auto &smtp_config = sc.GetConfig();

    try
    {
        Poco::Net::SocketAddress sa(smtp_config.host, smtp_config.port);
        Poco::Net::SecureStreamSocket socket(
            sa, Poco::Net::SSLManager::instance().defaultClientContext());
        Poco::Net::SMTPClientSession session(socket);
        session.login(Poco::Net::SMTPClientSession::AUTH_LOGIN, smtp_config.username,
                      smtp_config.password);
        Poco::Net::MailMessage message;
        std::string from_string = smtp_config.from_name + " <" + smtp_config.from_address + ">";
        message.setSender(from_string);
        message.addRecipient(
            Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, to));
        message.setSubject(Poco::Net::MailMessage::encodeWord("Mercury 验证码", "UTF-8"));
        message.setContent(BuildeEmailBodyForModifyPersonInfo(code));
        message.setContentType("text/html; charset=UTF-8");
        session.sendMessage(message);
        session.close();
    }
    catch (const Poco::Net::SMTPException &e)
    {
        throw common::StmpWrong("后台邮件线程发生Poco SMTP异常: ",
                                e.displayText()); // 向上抛出异常，让调用者知道失败了
    }
    catch (const Poco::Net::NetException &e)
    {
        throw common::PocoNetWrong("后台邮件线程发生Poco网络异常: ", e.displayText());
    }
    catch (const std::exception &e)
    {
        throw common::UnkownWrong("后台邮件线程发生未知异常: ", e.what());
    }
}

drogon::Task<Service::EmailServiceReturn> Service::EmailService::Verify(std::string email_address,
                                                                        std::string user_code)
{
    std::string rediskey = "verify:code:" + email_address;
    auto redis_client = drogon::app().getRedisClient();
    MY_LOG_INF("email_address", email_address);
    MY_LOG_INF("user_code", user_code);

    try
    {
        auto result = co_await redis_client->execCommandCoro("GET %s", rediskey.c_str());

        if (result.type() == drogon::nosql::RedisResultType::kNil)
        {
            EmailServiceReturn ret;
            ret.code = 400;
            ret.message = "验证码错误或已过期";
            co_return ret;
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
        throw common::RedisOperatorWrong("Redis 操作失败:", e.what());
    }
    catch (const std::exception &e)
    {
        throw common::UnkownWrong(e.what());
    }
}