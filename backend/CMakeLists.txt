cmake_minimum_required(VERSION 3.15)
project(mercury_kv VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}
  main.cc
  models/Users.cc
)

find_package(OpenSSL REQUIRED)
find_package(Drogon CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC
  Drogon::Drogon
  yaml-cpp::yaml-cpp
  OpenSSL::SSL
  OpenSSL::Crypto
)

target_include_directories(${PROJECT_NAME} PUBLIC
  .
  common
  models
)

if(NOT MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()
