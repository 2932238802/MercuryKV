#!/bin/bash
export PROJECT_ROOT=$(pwd)
export BACKEND_PORT=5555 
FRONTEND_DIR=$PROJECT_ROOT/frontend
BACKEND_DIR=$PROJECT_ROOT/backend
BACKEND_BUILD_DIR=$BACKEND_DIR/build
BACKEND_PUBLIC_DIR=$BACKEND_DIR/public 
EXECUTABLE_NAME=mercury_kv
VCPKG_TOOLCHAIN_FILE=/home/losangelous/LosAngelous/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
NGINX_CONF_FILE=$PROJECT_ROOT/nginx.conf

cleanup() {
    echo ""
    echo " "
    echo "========== 收到退出信号，开始清理... =========="
    echo "正在停止 Nginx..."
    sudo nginx -p "$PROJECT_ROOT" -c "$NGINX_CONF_FILE" -s stop
    echo "正在停止后端服务..."
    pkill -f $EXECUTABLE_NAME
    echo "清理完成！"
    exit 0
}

trap cleanup EXIT INT TERM
echo " -- 自动化构建与启动脚本开始 -- "
echo " "
echo "当前项目根目录设置为: $PROJECT_ROOT"
echo "后端服务端口设置为: $BACKEND_PORT"
echo " "

echo "========== 处理前端 =========="
echo "进入前端目录: $FRONTEND_DIR"
cd "$FRONTEND_DIR" || { echo "无法进入前端目录!"; exit 1; }

echo "安装前端依赖..."
npm install || { echo "依赖安装失败!"; exit 1; }

echo "打包前端代码至 dist 目录..."
npm run build || { echo "打包前端代码失败!"; exit 1; }

echo "检查 dist 目录是否存在..."
if [ ! -d "$FRONTEND_DIR/dist" ]; then
    echo "dist 目录未找到!"; exit 1;
fi

echo "清空并复制 dist 内容到后端 public 目录..."
mkdir -p "$BACKEND_PUBLIC_DIR"
rm -rf "$BACKEND_PUBLIC_DIR"/*
cp -r "$FRONTEND_DIR/dist"/* "$BACKEND_PUBLIC_DIR"/ || { echo "复制前端文件失败!"; exit 1; }
echo "========== 前端处理完毕 =========="
echo " "

echo "========== 处理后端 =========="
echo "清理并重建后端构建目录..."
rm -rf "$BACKEND_BUILD_DIR"
mkdir -p "$BACKEND_BUILD_DIR"
cd "$BACKEND_BUILD_DIR" || { echo "无法进入后端构建目录!"; exit 1; }

echo "使用 CMake 生成 Makefile ..."
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_TOOLCHAIN_FILE -DBUILD_REDIS=ON || { echo "CMake 配置失败!"; exit 1; }

echo "使用 Make 编译后端项目..."
make -j$(nproc) || { echo "编译后端失败!"; exit 1; }
echo "========== 后端处理完毕 =========="
echo " "

echo "========== 启动服务 =========="

if [ ! -f "$NGINX_CONF_FILE" ]; then
    echo "错误: Nginx 配置文件未找到于 $NGINX_CONF_FILE"
    exit 1
fi

echo "正在启动 OpenResty/Nginx 作为反向代理..."
sudo nginx -p "$PROJECT_ROOT" -c "$NGINX_CONF_FILE" -s stop || true 
sudo nginx -p "$PROJECT_ROOT" -c "$NGINX_CONF_FILE"

echo "Nginx 已在后台启动。"
echo " "

echo "启动后端 C++ 服务... (按 Ctrl+C 可随时停止所有服务)"
cd "$BACKEND_BUILD_DIR"

stdbuf -o0 -e0 ./$EXECUTABLE_NAME &

wait $!
echo " "
echo " -- 脚本执行完毕 -- "
