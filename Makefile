.PHONY = compile-prod format compile compile-test clean lint test test-memory

BUILD_DIR = build
BUILD_DIR_PROD = $(BUILD_DIR)/prod
BUILD_DIR_TEST = $(BUILD_DIR)/test

compile-prod:
	@cmake -S GPTifier -B $(BUILD_DIR_PROD)
	@make --jobs=12 --directory=$(BUILD_DIR_PROD) install

format:
	@clang-format -i --verbose --style=file \
		GPTifier/src/*.cpp GPTifier/src/*/*.cpp \
		GPTifier/src/*.hpp GPTifier/src/*/*.hpp

compile: format compile-prod

compile-test: format # Private target
	@cmake -S GPTifier -B $(BUILD_DIR_TEST) -DENABLE_TESTING=ON
	@make --jobs=12 --directory=$(BUILD_DIR_TEST)

clean:
	@rm -rfv $(BUILD_DIR)

lint:
	@cppcheck GPTifier --enable=all

test: export PATH_BIN = $(CURDIR)/$(BUILD_DIR_TEST)/gpt
test: compile-test
	@python3 -m unittest -v tests/*.py

test-memory: export PATH_BIN = $(CURDIR)/$(BUILD_DIR_TEST)/gpt
test-memory: export TEST_MEMORY = 1
test-memory: compile-test
	@python3 -m unittest -v tests/*.py
