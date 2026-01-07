.PHONY = compile-prod format compile tidy clean lint compile-test test test-memory py

BUILD_DIR = build
BUILD_DIR_PROD = $(BUILD_DIR)/prod
BUILD_DIR_TEST = $(BUILD_DIR)/test
PATH_BIN_TEST = $(CURDIR)/$(BUILD_DIR_TEST)/gpt

compile-prod:
	@cmake -S GPTifier -B $(BUILD_DIR_PROD)
	@make --jobs=12 --directory=$(BUILD_DIR_PROD) install

format:
	@clang-format -i --verbose --style=file \
		GPTifier/src/*.cpp GPTifier/src/*/*.cpp \
		GPTifier/src/*.hpp GPTifier/src/*/*.hpp

compile: format compile-prod

tidy:
	@cmake -S GPTifier -B $(BUILD_DIR_PROD)
	@clang-tidy -p $(BUILD_DIR_PROD) \
		GPTifier/src/*.cpp GPTifier/src/*/*.cpp \
		GPTifier/src/*.hpp GPTifier/src/*/*.hpp

clean:
	@rm -rfv $(BUILD_DIR)

lint:
	@cppcheck GPTifier --enable=all

compile-test: format # Private target
	@cmake -S GPTifier -B $(BUILD_DIR_TEST) -DENABLE_TESTING=ON -DENABLE_COVERAGE=ON
	@make --jobs=12 --directory=$(BUILD_DIR_TEST)

test: export PATH_BIN = $(PATH_BIN_TEST)
test: compile-test
	-@python3 -m pytest -vs tests/
	@lcov --quiet --capture --directory=$(BUILD_DIR_TEST) --output-file $(BUILD_DIR_TEST)/coverage.info
	@lcov --quiet --remove $(BUILD_DIR_TEST)/coverage.info "/usr/*" "*/external/*" --output-file $(BUILD_DIR_TEST)/coverage.info
	@genhtml --quiet $(BUILD_DIR_TEST)/coverage.info --output-directory $(BUILD_DIR_TEST)/coverageResults
	@echo "See coverage report at: $(CURDIR)/$(BUILD_DIR_TEST)/coverageResults/index.html"

test-memory: export PATH_BIN = $(PATH_BIN_TEST)
test-memory: export TEST_MEMORY = 1
test-memory: compile-test
	@python3 -m unittest -v tests/*.py

py:
	@black tests/*.py
	@pylint --exit-zero tests/*.py
	@mypy --strict tests/*.py
