#include "CheckTokenService/CheckTokenService.hpp"

/**
 * @brief token验证
 *
 * @param token
 * @return drogon::Task<Service::CheckTokenReturn>
 */
drogon::Task<Service::CheckTokenReturn> Service::CheckTokenService::Check(std::string &&token_out)
{
    const std::string bearer_prefix = "Bearer ";
    std::string token = "";
    if (!token_out.empty() && token_out.find(bearer_prefix, 0) == 0)
    {
        token = token_out.substr(bearer_prefix.length());
    }

    if (token.empty())
    {
        throw Service::RequestWrong("token 内容为空 请求错误");
    }

    auto user_id_optional = common::MyJWT::Verufyjwt(token);

    if (!user_id_optional)
    {
        throw Service::AuthException("认证失败，Token 无效或已过期");
    }

    try
    {
        drogon::orm::Mapper<drogon_model::mercury::Users> mapper_users(db_client);

        int64_t user_id = std::stoll(*user_id_optional); // user_id
        MY_LOG_INF("user_id:", user_id);
        std::string sql = "SELECT * FROM users WHERE user_id = \$1";
        auto result = co_await db_client->execSqlCoro(sql, user_id);
        if (result.empty())
        {
            // 如果是空的
            throw Service::AuthException("认证失败，用户不存在");
        }

        drogon_model::mercury::Users user(result[0]);

        CheckTokenReturn ret;
        ret.code = 200;
        ret.message = "欢迎回来";
        ret.user_id = user_id;
        ret.username = user.getValueOfUsername();
        co_return ret;
    }
    catch (const drogon::orm::DrogonDbException &e)
    {
        MY_LOG_ERROR("数据库查询异常: ", e.base().what());
        throw Service::DBOperatorWrong("数据库服务异常");
    }
    catch (const std::exception &e)
    {
        MY_LOG_ERROR("Token 验证服务发生内部错误: ", e.what());
        throw Service::DBOperatorWrong("服务内部错误");
    }
}
