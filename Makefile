.PHONY = help format compile clean lint test test-memory
.DEFAULT_GOAL = help

BUILD_DIR = build
BUILD_DIR_PROD = $(BUILD_DIR)/prod
BUILD_DIR_TEST = $(BUILD_DIR)/test

define HELP_LIST_TARGETS
To format code:
    $$ make format
To compile binary:
    $$ make compile
To remove build directory:
    $$ make clean
To run cppcheck linter:
    $$ make lint
To run unit tests:
    $$ make test
To run Valgrind unit tests:
    $$ make test-memory
endef

export HELP_LIST_TARGETS

help:
	@echo "$$HELP_LIST_TARGETS"

format:
	@clang-format -i --verbose --style=file GPTifier/src/*.cpp GPTifier/include/*.hpp

compile: format
	@cmake -S GPTifier -B $(BUILD_DIR_PROD)
	@make --jobs=12 --directory=$(BUILD_DIR_PROD) install

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
