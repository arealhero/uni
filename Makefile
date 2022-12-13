GENERATOR := Ninja
BUILD_DIR := Build

BUILD_TYPE ?= Debug
TOOLCHAIN_FILE ?= Meta/CMake/Toolchains/clang.cmake
USE_CLANG_TIDY ?= OFF

all: setup
	@cmake --build $(BUILD_DIR) --target Applications Libraries

docs: setup
	@cmake --build $(BUILD_DIR) --target Latex

apps: setup
	@cmake --build $(BUILD_DIR) --target Applications

libs: setup
	@cmake --build $(BUILD_DIR) --target Libraries

tests: setup
	@cmake --build $(BUILD_DIR) --target Tests
	@$(BUILD_DIR)/Tests/Tests

setup:
	@cmake -S . -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
							-DUSE_CLANG_TIDY=$(USE_CLANG_TIDY) \
							-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
							-B $(BUILD_DIR) \
							-G $(GENERATOR)

clean:
	@$(RM) -r $(BUILD_DIR)

.PHONY: all docs apps libs tests setup
