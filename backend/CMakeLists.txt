cmake_minimum_required(VERSION 3.20)
project(mercury_kv VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TOOLCHAIN_FILE "/home/losangelous/LosAngelous/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

set(APP_SOURCES
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
  controllers/EmailVerifyApi/VerifyAndModify.cc
  services/LoginService/LoginService.cc
  services/RegisterService/RegisterService.cc
  services/CheckTokenService/CheckTokenService.cc
  services/AlterService/AddDataService.cc
  services/AlterService/AlterDataService.cc
  services/AlterService/DeleteDataService.cc
  services/EmailService/EmailService.cc
  services/AlterPersonInfoService/AlterPersonInfo.cc
  utils/ConfigManage/ConfigManage.cc
  utils/SmptUtil/SmptUtil.cc
)


find_package(Drogon CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation Net XML Util NetSSL)

add_executable(${PROJECT_NAME} ${APP_SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -fcoroutines)
target_compile_definitions(${PROJECT_NAME} PRIVATE USE_COROUTINE)
target_include_directories(${PROJECT_NAME} PUBLIC
  common
  models
  controllers
  services
  plugins 
  utils
)

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
  Poco::NetSSL
  CURL::libcurl
)


# if(NOT MSVC)
#   target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
# endif()


if(NOT MSVC)
  add_compile_options(-march=x86-64)
  add_compile_options(-mno-avx -mno-avx2 -mno-avx512f)
  add_compile_options(-msse2)  # 只使用 SSE2，这在几乎所有现代 CPU 上都支持
endif()