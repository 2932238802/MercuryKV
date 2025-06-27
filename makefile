BACKEND_DIR = ./backend
BUILD_DIR = $(BACKEND_DIR)/build

.PHONY: all build rebuild clean check
all: build

rebuild: clean build 

build:
	@echo "--- [1/2] Preparing build directory..."
	@mkdir -p $(BUILD_DIR)
	@echo "--- [2/2] Configuring and Compiling Backend..."
	@cd $(BUILD_DIR) && cmake -D CMAKE_TOOLCHAIN_FILE=/home/losangelous/LosAngelous/dev/vcpkg/scripts/buildsystems/vcpkg.cmake .. && cmake --build . 
	@echo "--- Backend build complete! Executable is in $(BUILD_DIR) ---"

clean:
	@echo "--- Cleaning up build directory..."
	@rm -rf $(BUILD_DIR)

check:
	@tree -L 3 -I "node_modules"
