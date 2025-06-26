# ==============================================================================
# 1. 项目定义与基本设置 (极简版)
# ==============================================================================
cmake_minimum_required(VERSION 3.15)
project(mercury_kv_test VERSION 1.0.0 LANGUAGES CXX)

# [关键!] 告诉 CMake 生成 compile_commands.json，这是为了让 clangd 等编辑器工具能找到头文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==============================================================================
# 2. C++ 标准设置
# ==============================================================================
# Drogon 需要 C++17 或更高版本，我们直接设置为 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "项目 ${PROJECT_NAME} 使用 C++${CMAKE_CXX_STANDARD} 标准进行编译")

# ==============================================================================
# 3. 创建可执行文件并链接 Drogon
# ==============================================================================
# 创建一个名为 "mercury_kv_test" 的可执行文件，源文件只有 main.cc
add_executable(${PROJECT_NAME} main.cc)

# 查找 Drogon 框架的配置
# 如果这步失败，说明 Drogon 没有被正确安装或 vcpkg 工具链没有配置好
find_package(Drogon CONFIG REQUIRED)

# [核心步骤!] 将 Drogon 链接到我们的可执行文件
# 这一行命令会自动处理好链接库、头文件包含路径等所有事情
target_link_libraries(${PROJECT_NAME} PUBLIC Drogon::Drogon)

# (可选) 添加一些基本的编译器警告
if(NOT MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

message(STATUS "Drogon 已成功链接到目标 ${PROJECT_NAME}")

# ==============================================================================
# 4. 安装设置 (可选，但建议保留)
# ==============================================================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
