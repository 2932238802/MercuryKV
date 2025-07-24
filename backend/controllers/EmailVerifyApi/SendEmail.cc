#include "EmailVerify.h"
#include "MyLog.hpp"
#include "Type.hpp"
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/SqlBinder.h>

drogon::Task<drogon::HttpResponsePtr> EmailVerify::SendEmail(drogon::HttpRequestPtr req)
{
    Json::Value ret;
    std::string email_address = req->getParameter("email");
    MY_LOG_INF("应该发送的邮件地址是:", email_address);
    try
    {
        auto redis_client = drogon::app().getRedisClient();
        std::string code = common::RandStr(6); // 生成验证码
        std::string rediskey = "verify:code:" + email_address;
        auto redis_res = co_await redis_client->execCommandCoro("SET %s %s EX 600",
                                                                rediskey.c_str(), code.c_str());

        if (redis_res.asString() == "OK")
        {
            MY_LOG_SUC("redis 数据设置成功");
        }

        static std::atomic<size_t> io_loop_counter = {0};
        size_t num_io_threads = drogon::app().getThreadNum();
        MY_LOG_INF("已经生成六位数的验证码", code);
        MY_LOG_INF("num_io_threads 是 ", num_io_threads);

        if (num_io_threads > 0)
        {
            size_t loop_index = io_loop_counter++ % num_io_threads;
            auto io_loop = drogon::app().getIOLoop(loop_index);
            if (io_loop)
            {
                io_loop->runInLoop(
                    [email = email_address, verification_code = code]
                    {
                        try
                        {
                            Service::EmailService::SendEmail(email, verification_code);
                            MY_LOG_INF("后台邮件发送任务已提交至 I/O 线程",
                                       drogon::app().getCurrentThreadIndex(), ",目标地址");
                        }
                        catch (const common::StmpWrong &e)
                        {
                            MY_LOG_ERROR(e.what());
                        }
                        catch (const common::PocoNetWrong &e)
                        {
                            MY_LOG_ERROR(e.what());
                        }
                        catch (const common::UnkownWrong &e)
                        {
                            MY_LOG_ERROR(e.what());
                        }
                    });
                MY_LOG_INF("验证码已经发送");
                ret["message"] = "验证码已经发送";
                ret["code"] = 200;
                auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
                co_return resp;
            }
            else
            {
                LOG_ERROR << "无法获取 I/O Loop,邮件发送任务无法在后台执行!";
                ret["error"] = "无法获取 I/O Loop,邮件发送任务无法在后台执行!";
                auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
                co_return resp;
            }
        }
    }
    catch (const common::PocoNetWrong &e)
    {
        LOG_ERROR << "Poco 连接问题:" << e.what();
        ret["error"] = e.what();
        auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(drogon::k500InternalServerError);
        co_return resp;
    }
    catch (const common::StmpWrong &e)
    {
        LOG_ERROR << "Stmp的问题:" << e.what();
        ret["error"] = "Stmp的问题";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(drogon::k500InternalServerError);
        co_return resp;
    }
    catch (const drogon::nosql::RedisException &e)
    {
        LOG_ERROR << "Redis 失败" << e.what();
        ret["error"] = e.what();
        auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(drogon::k500InternalServerError);
        co_return resp;
    }
    catch (const common::UnkownWrong &e)
    {
        // 捕获其他所有异常
        LOG_ERROR << "发生一个未知错误: " << e.what();
        ret["error"] = "发生一个未知错误";
        auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
        resp->setStatusCode(drogon::k500InternalServerError);
        co_return resp;
    }
}
