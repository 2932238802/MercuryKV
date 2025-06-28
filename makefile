BACKEND_DIR = ./backend
BUILD_DIR = $(BACKEND_DIR)/build
VCPKG_TOOLCHAIN_FILE = /home/losangelous/LosAngelous/dev/vcpkg/scripts/buildsystems/vcpkg.cmake

.PHONY: all build rebuild clean check

all: build

rebuild: clean build

build:
	@echo "--- [1/2] 正在使用 CMake 和 Vcpkg 配置后端..."
	@cmake -S $(BACKEND_DIR) -B $(BUILD_DIR) -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN_FILE)
	
	@echo "--- [2/2] 正在编译后端..."
	@cmake --build $(BUILD_DIR) -- -j$(nproc)
	
	@echo "--- 后端构建完成！可执行文件位于 $(BUILD_DIR) ---"

clean:
	@echo "--- 正在清理构建目录..."
	@rm -rf $(BUILD_DIR)

check:
	@tree -L 3 -I "node_modules"

