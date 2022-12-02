GENERATOR := Ninja

apps: setup
	@cmake --build Build --target Applications

libs: setup
	@cmake --build Build --target Libraries

tests: setup
	@cmake --build Build --target Tests
	@Build/Tests/Tests

setup:
	@cmake -S . -B Build -G $(GENERATOR)

.PHONY: libs tests setup
