#pragma once
#include "MyCrypt.hpp"
#include "MyJWT.hpp"
#include "MyLog.hpp"
#include "RegisterService.hpp"
#include "Users/Users.h"
#include "drogon/orm/DbClient.h"
#include "type.hpp"
#include <cstddef>
#include <cstdint>
#include <drogon/orm/Exception.h>
#include <exception>
#include <json/value.h>
#include <memory>

namespace Service
{
// ----- ----- ----- ----- -----
// 设置成功返回的数据结构
struct RegisterResult
{
    std::string message;
    int64_t code;
    std::string token;
    int64_t user_id;
    std::string username;
    std::string email;
};

// ----- ----- ----- ----- -----
/**
 * @brief 继承父类的 std::runtime_error 的错误
 * explicit 防止隐式类型转换
 */
class RegisterServiceFactory;
class RegisterService
{

  public:
    using ptr = std::shared_ptr<RegisterService>;
    virtual drogon::Task<RegisterResult> Register(const Json::Value &req);
    friend class LoginServiceFactory;
    drogon::orm::DbClientPtr db_client; // std::shared_ptr<DbClient>

  protected: // 私有构造函数
    RegisterService(drogon::orm::DbClientPtr db_out);
};

class RegisterServiceFactory
{
  public:
    static RegisterService::ptr MakeService(drogon::orm::DbClientPtr db_out)
    {
        // 虽然是友元 但是 make_shared 还是访问不了私有构造
        // 可以改成 new LoginService(db_out) 但是不够优雅
        // 推荐的方式 就是 使用 辅助类去构造

        struct Enable : public RegisterService
        {
            Enable(drogon::orm::DbClientPtr db_out) : RegisterService(db_out)
            {
            }
        };

        // 子类到父类的隐式类型转换
        // 指针或引用转换（不发生截断）
        // 赋值会发生截断
        return std::make_shared<Enable>(db_out);
    }
};

} // namespace Service
