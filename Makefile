GENERATOR := Ninja
BUILD_TYPE ?= Debug
BUILD_DIR := Build

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
	@cmake -S . -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -B $(BUILD_DIR) -G $(GENERATOR)

clear:
	@$(RM) -r $(BUILD_DIR)

.PHONY: all apps libs tests setup
