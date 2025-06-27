#pragma once

#include <drogon/HttpSimpleController.h>
#include <drogon/HttpTypes.h>

using namespace drogon;

// 简单控制器
/**
 * @brief 工具类 处理登录清晰 然后对登录信息进行查询
 * 1. 主要业务：  asyncHandleHttpRequest 处理登录请求
 * 2.
 *
 */
class Login : public drogon::HttpSimpleController<Login> {
public:
  void asyncHandleHttpRequest(
      const HttpRequestPtr &req,
      std::function<void(const HttpResponsePtr &)> &&callback) override;
  PATH_LIST_BEGIN
  // list path definitions here;
  // PATH_ADD("/path", "filter1", "filter2", HttpMethod1, HttpMethod2...);

  // post 请求发送
  PATH_ADD("/login", Post);
  PATH_LIST_END
};
