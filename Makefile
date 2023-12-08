.PHONY = help compile clean lint test test-memory format
.DEFAULT_GOAL = help

define HELP_LIST_TARGETS
To compile binary:
    $$ make compile
To remove build directory:
    $$ make clean
To run cppcheck linter:
    $$ make lint
To run unit tests:
    $$ make test
To run Valgrind tests:
    $$ make test-memory
To format code:
    $$ make format
endef

export HELP_LIST_TARGETS

help:
	@echo "$$HELP_LIST_TARGETS"

compile:
	@cmake -S GPTifier -B build
	@make --jobs=12 --directory=build install

clean:
	@rm -rfv build

lint:
	@cppcheck GPTifier --enable=all

test: compile
	@python3 -m pytest -vs tests -m "not valgrind"

test-memory: compile
	@python3 -m pytest -vs tests -m "valgrind"

format:
	@clang-format -i --verbose --style=file GPTifier/src/*.cpp GPTifier/include/*.hpp
