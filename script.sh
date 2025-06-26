#!/bin/bash

# PROJECT_DIR=/home/losangelous/LosAngelous/Project/MercuryKV
# FRONTEND_DIR=$PROJECT_DIR/frontend
# BACKEND_DIR=$PROJECT_DIR/backend

# echo " -- 脚本开始运行 -- "
# echo " -- 进入前端目录 -- "
# cd $FRONTEND_DIR

# echo " -- 安装前端依赖 -- "
# npm install || {
#     echo " -- 依赖安装失败! -- "
#     exit 1
# }

# echo " -- 打包前端代码 至dist -- "
# npm run build || {
#     echo " -- 打包前端代码失败! -- "
# exit 1
# }

# echo " -- 前端代码处理完成 -- "
# if [ ! -d "$FRONTEND_DIR/dist" ]; then
#     echo " -- dist 文件丢失 -- "
#     exit 1
# fi
# cp -r $FRONTEND_DIR/dist/* $BACKEND_DIR/public/ || {
#     echo "Error: Failed to copy Vue files"
#     exit 1
# }
# echo " -- 前端代码打包至后端成功 -- "







PROJECT_DIR=/home/losangelous/LosAngelous/Project/MercuryKV
FRONTEND_DIR=$PROJECT_DIR/frontend
BACKEND_DIR=$PROJECT_DIR/backend
BACKEND_BUILD_DIR=$BACKEND_DIR/build # 后端构建目录
EXECUTABLE_NAME=mercury_kv          

echo " -- 脚本开始运行 -- "

echo " "
echo "========== 1. 处理前端 =========="
echo "进入前端目录: $FRONTEND_DIR"
cd $FRONTEND_DIR

echo "安装前端依赖..."
npm install || { echo "依赖安装失败!"; exit 1; }

echo "打包前端代码至 dist 目录..."
npm run build || { echo "打包前端代码失败!"; exit 1; }

echo "检查 dist 目录是否存在..."
if [ ! -d "$FRONTEND_DIR/dist" ]; then
    echo "dist 目录未找到!"; exit 1;
fi

# echo "创建后端 public 目录 (如果不存在)..."
# mkdir -p $BACKEND_DIR/public 

echo "复制 dist 内容到后端 public 目录..."
cp -r $FRONTEND_DIR/dist/* $BACKEND_DIR/public/ || { echo " 复制前端文件失败!"; exit 1; }

echo "前端部分处理完成！"

echo " "
echo "========== 2. 处理后端 =========="
echo "进入后端构建目录: $BACKEND_BUILD_DIR"

rm build && mkdir build
cd $BACKEND_BUILD_DIR

echo "使用 CMake 生成 Makefile..."
cmake .. || { echo " CMake 配置失败!"; exit 1; }

echo "使用 Make 编译后端项目..."
make || { echo "编译后端失败!"; exit 1; }

echo "后端编译完成！可执行文件位于: $BACKEND_BUILD_DIR/$EXECUTABLE_NAME"

echo " "
echo "========== 3. 启动服务 =========="
echo "运行后端程序..."
./$EXECUTABLE_NAME
echo " "
echo " -- 后端程序已停止，脚本执行完毕 -- "
