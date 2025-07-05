#!/bin/bash

GREEN='\033[0;32m'
YELLOW='\033[1;33m'

# 这个恢复正常一下
NC='\033[0m'

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
FRONTEND_DIR="$SCRIPT_DIR/frontend"

echo -e "${GREEN}--- 启动前端开发服务器 ---${NC}"
echo ""

if [ ! -d "$FRONTEND_DIR" ]; then
    echo -e "${YELLOW}错误: 前端目录 '$FRONTEND_DIR' 未找到!${NC}"
    exit 1
fi

echo "进入前端目录: $FRONTEND_DIR"
cd "$FRONTEND_DIR" || exit 1

npm run dev

