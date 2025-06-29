#!/bin/bash


PROJECT_DIR=/home/losangelous/LosAngelous/Project/MercuryKV
FRONTEND_DIR=$PROJECT_DIR/frontend
BACKEND_DIR=$PROJECT_DIR/backend
BACKEND_BUILD_DIR=$BACKEND_DIR/build
EXECUTABLE_NAME=mercury_kv
VCPKG_TOOLCHAIN_FILE=/home/losangelous/LosAngelous/dev/vcpkg/scripts/buildsystems/vcpkg.cmake

echo " -- 脚本开始运行 -- "
echo " "
echo "========== 处理前端 =========="
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


echo "复制 dist 内容到后端 public 目录..."
rm -r $BACKEND_DIR/public/*
cp -r $FRONTEND_DIR/dist/* $BACKEND_DIR/public/ || { echo " 复制前端文件失败!"; exit 1; }

echo "前端部分处理完成！"

echo " "
echo "========== 处理后端 =========="
echo "清理并重建后端构建目录..."

rm -rf $BACKEND_BUILD_DIR
mkdir -p $BACKEND_BUILD_DIR
cd $BACKEND_BUILD_DIR


echo "使用 CMake 生成 Makefile ..."
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_TOOLCHAIN_FILE -DBUILD_REDIS=ON || { echo " CMake 配置失败!"; exit 1; }

echo "使用 Make 编译后端项目..."
make -j$(nproc) || { echo "编译后端失败!"; exit 1; } # 增加了 -j$(nproc) 来加速编译

echo "后端编译完成！可执行文件位于: $BACKEND_BUILD_DIR/$EXECUTABLE_NAME"

echo " "
echo "========== 3. 启动服务 =========="
echo "运行后端程序..."
stdbuf -o0 -e0 ./$EXECUTABLE_NAME
echo " "
echo " -- 后端程序已停止，脚本执行完毕 -- "