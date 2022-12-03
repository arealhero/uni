GENERATOR := Ninja
BUILD_TYPE ?= Debug

all: setup
	@cmake --build Build --target Applications Libraries

apps: setup
	@cmake --build Build --target Applications

libs: setup
	@cmake --build Build --target Libraries

tests: setup
	@cmake --build Build --target Tests
	@Build/Tests/Tests

setup:
	@cmake -S . -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -B Build -G $(GENERATOR)

.PHONY: all apps libs tests setup
