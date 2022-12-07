GENERATOR := Ninja
BUILD_DIR := Build

BUILD_TYPE ?= Debug
TOOLCHAIN_FILE ?= Meta/CMake/Toolchains/clang.cmake

all: setup
	@cmake --build $(BUILD_DIR) --target Applications Libraries

apps: setup
	@cmake --build $(BUILD_DIR) --target Applications

libs: setup
	@cmake --build $(BUILD_DIR) --target Libraries

tests: setup
	@cmake --build $(BUILD_DIR) --target Tests
	@$(BUILD_DIR)/Tests/Tests

setup:
	@cmake -S . -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -B $(BUILD_DIR) -G $(GENERATOR)

clean:
	@$(RM) -r $(BUILD_DIR)

.PHONY: all apps libs tests setup
