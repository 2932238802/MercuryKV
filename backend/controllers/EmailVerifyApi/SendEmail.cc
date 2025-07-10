

#include "EmailVerify.h"

/**
 * @brief 发送验证码
 *
 * @param req
 * @param callback
 * @param email_address
 */
void EmailVerify::SendEmail(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                            std::string email_address) const
{
}