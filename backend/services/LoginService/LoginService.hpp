#pragma once
#include "MyCrypt.hpp"
#include "MyJWT.hpp"
#include "MyLog.hpp"
#include "Type.hpp"
#include "Users/Users.h"
#include "drogon/orm/DbClient.h"
#include <cstddef>
#include <cstdint>
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/Criteria.h>
#include <drogon/orm/DbClient.h>
#include <drogon/orm/Mapper.h>
#include <drogon/utils/coroutine.h>
#include <memory>
namespace Service
{

// ----- ----- ----- ----- -----
// 设置成功返回的数据结构
struct LoginResult
{
    // std::string username; 这个 controller直接处理就好了
    size_t code;
    int64_t user_id;
    std::string email;
    std::string token;
    std::string message;
};

// ----- ----- ----- ----- -----
/**
 * @brief 继承父类的 std::runtime_error 的错误
 * explicit 防止隐式类型转换
 */
class LoginServiceFactory;
class LoginService
{

  public:
    using ptr = std::shared_ptr<LoginService>;
    virtual drogon::Task<LoginResult> LoginVerify(const std::string &username,
                                                  const std::string &password);
    friend class LoginServiceFactory;
    drogon::orm::DbClientPtr db_client; // std::shared_ptr<DbClient>

  protected: // 私有构造函数
    LoginService(drogon::orm::DbClientPtr db_out);
};

class LoginServiceFactory
{
  public:
    static LoginService::ptr MakeService(drogon::orm::DbClientPtr db_out)
    {
        // 虽然是友元 但是 make_shared 还是访问不了私有构造
        // 可以改成 new LoginService(db_out) 但是不够优雅
        // 推荐的方式 就是 使用 辅助类去构造

        struct Enable : public LoginService
        {
            Enable(drogon::orm::DbClientPtr db_out) : LoginService(db_out)
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
