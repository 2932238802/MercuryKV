#!/bin/bash

PROJECT_ROOT=$(pwd)
EXECUTABLE_NAME=mercury_kv
BACKEND_DIR=$PROJECT_ROOT/mercury
BACKEND_BUILD_DIR=$BACKEND_DIR/build

echo "========== 正在准备后端开发环境... =========="
cd backend
mkdir -p build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=... # 你的cmake命令
make -j$(nproc)

echo ""
echo "✅ 后端准备就绪！"
echo "✅ 开始启动 ~ "
cd "$BACKEND_BUILD_DIR"
stdbuf -o0 -e0 ./$EXECUTABLE_NAME
