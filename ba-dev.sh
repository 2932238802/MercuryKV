#!/bin/bash

set -eu

PROJECT_ROOT=$(pwd)
EXECUTABLE_NAME="mercury_kv"

BACKEND_DIR="$PROJECT_ROOT/backend" 
BACKEND_BUILD_DIR="$BACKEND_DIR/build"
VCPKG_TOOLCHAIN_FILE="/home/losangelous/LosAngelous/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"

echo "========== 正在准备后端开发环境... =========="

# 检查后端目录是否存在
if [ ! -d "$BACKEND_DIR" ]; then
    echo "❌ 错误：找不到后端目录 '$BACKEND_DIR'！请检查目录名是否正确。"
    exit 1
fi

# 检查工具链文件是否存在
if [ ! -f "$VCPKG_TOOLCHAIN_FILE" ]; then
    echo "❌ 错误：找不到 vcpkg 工具链文件 '$VCPKG_TOOLCHAIN_FILE'！请检查路径是否正确。"
    exit 1
fi

# 进入后端目录并创建 build 文件夹
cd "$BACKEND_DIR"
mkdir -p build
cd build

echo "--> 正在运行 CMake..."
# 使用修正后的 cmake 命令
cmake .. -DCMAKE_TOOLCHAIN_FILE="$VCPKG_TOOLCHAIN_FILE"

echo "--> 正在编译项目..."
# 使用 nproc 获取可用的 CPU 核心数来并行编译，加快速度
make -j$(nproc)

echo ""
echo "✅ 后端编译完成！"
echo "🚀 开始启动应用..."
echo "-------------------------------------"

# 修正：确保我们是在正确的 build 目录里执行程序
# （因为前面的 cd build 已经进入了，所以这一步是双重保险）
cd "$BACKEND_BUILD_DIR"
# 使用 stdbuf 确保日志能实时输出
stdbuf -o0 -e0 ./$EXECUTABLE_NAME
