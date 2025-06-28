#pragma once
#include <chrono>
#include <drogon/HttpAppFramework.h>
#include <jwt-cpp/jwt.h>
#include <string>

using namespace drogon;

namespace MyJWTNS {

class MyJWT {
public:
  static std::string GetJWT(const std::string &user_id) {
    const auto &jwt_config = app().getCustomConfig()["jwt"];
    std::string secret_key = jwt_config["secret_key"].asString();
    std::string issuer = jwt_config["issuer"].asString();
    long expiry_hours = jwt_config["expiry_hours"].asInt64();
    std::string token = jwt::create()
                            .set_issuer(issuer)
                            .set_subject(user_id)
                            .set_issued_at(std::chrono::system_clock::now())
                            .set_expires_at(std::chrono::system_clock::now() +
                                            std::chrono::hours{expiry_hours})
                            .sign(jwt::algorithm::hs256{secret_key});
    return token;
  }
};

} // namespace MyJWTNS
