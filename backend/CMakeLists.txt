cmake_minimum_required(VERSION 3.20)
project(mercury_kv VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TOOLCHAIN_FILE "/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
add_compile_options(-fcoroutines)
add_executable(${PROJECT_NAME}
  main.cc

  models/Users/Users.cc
  models/ApiKey/ApiKeys.cc
  models/KV_Tag/KvTagAssociation.cc
  models/KVStore/KvStore.cc
  models/Tags/Tags.cc

  controllers/LoginApi/Login.cc
  controllers/RegisterApi/Register.cc
  controllers/FetchDataApi/FetchData.cc
  controllers/AlterApi/AddData.cc
  controllers/AlterApi/DeleteData.cc
  controllers/AlterApi/UpdateData.cc
  controllers/EmailVerifyApi/SendEmail.cc
  controllers/EmailVerifyApi/Verify.cc

  services/LoginService/LoginService.cc
  services/RegisterService/RegisterService.cc
  services/CheckTokenService/CheckTokenService.cc
  services/AlterService/AddDataService.cc
  services/AlterService/AlterDataService.cc
  services/AlterService/DeleteDataService.cc
  services/EmailService/EmailService.cc
  services/AlterPersonInfo/AlterPersonInfo.cc

  plugins/SmptPlugin/SmptPlugin.cc

)

find_package(OpenSSL REQUIRED)
find_package(Drogon CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(Poco REQUIRED COMPONENTS
  Foundation
  Net
  XML
  Util
)

# 链接库
target_link_libraries(${PROJECT_NAME} PUBLIC
  Drogon::Drogon
  yaml-cpp::yaml-cpp
  OpenSSL::SSL
  OpenSSL::Crypto
  jwt-cpp::jwt-cpp
  Poco::Foundation
  Poco::Net
  Poco::XML
  Poco::Util
  CURL::libcurl
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME} PUBLIC
  common
  models
  controllers
  services
  plugins
)

# 启用 Drogon 协程支持（根据 Drogon 文档，某些版本需要此宏）
target_compile_definitions(${PROJECT_NAME} PRIVATE USE_COROUTINE)

# 非 MSVC 平台添加警告选项
if(NOT MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()
