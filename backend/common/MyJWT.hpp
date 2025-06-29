#pragma once
#include "MyLog.hpp"
#include <chrono>
#include <drogon/HttpAppFramework.h>
#include <jwt-cpp/jwt.h>
#include <optional>
#include <string>

using namespace drogon;
using namespace MyLogNS;
namespace MyJWTNS {

class MyJWT {
public:
  static std::string GetJWT(const std::string &user_id) {
    // 从配置文件 读取密钥
    const auto &jwt_config = app().getCustomConfig()["jwt"];
    std::string secret_key = jwt_config["secret_key"].asString();
    std::string issuer = jwt_config["issuer"].asString();
    long expiry_hours = jwt_config["expiry_hours"].asInt64();

    // 生成哈希加密
    std::string token = jwt::create()
                            .set_issuer(issuer)
                            .set_subject(user_id)
                            .set_issued_at(std::chrono::system_clock::now())
                            .set_expires_at(std::chrono::system_clock::now() +
                                            std::chrono::hours{expiry_hours})
                            .sign(jwt::algorithm::hs256{secret_key});
    return token;
  }

public:
  static std::optional<std::string> Verufyjwt(const std::string &token) {

    const auto &jwt_config = app().getCustomConfig()["jwt"];
    std::string secret_key = jwt_config["secret_key"].asString();
    std::string issuer = jwt_config["issuer"].asString();
    long expiry_hours = jwt_config["expiry_hours"].asInt64();

    // 验证一下 token是不是对的
    // 解密并装载
    try {
      auto decode = jwt::decode(token);

      auto verifier = jwt::verify()
                          .leeway(expiry_hours)
                          .with_issuer(issuer)
                          .allow_algorithm(jwt::algorithm::hs256{secret_key});

      // 下面这行代码会抛出异常这会抛出异常
      verifier.verify(decode);

      return decode.get_subject();
    } catch (const jwt::error::token_verification_exception &e) {
      MY_LOG_WARN("JWT verification failed: ", e.what());
      return std::nullopt;
    }

    catch (const std::exception &e) {
      // 捕获其他异常 (如 token 格式本身就无效，导致 jwt::decode 失败)

      MY_LOG_WARN("An unexpected error occurred during JWT verification:  ",
                  e.what());
      return std::nullopt;
    }
  }
};

} // namespace MyJWTNS
