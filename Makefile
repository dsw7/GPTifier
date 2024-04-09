.PHONY = help compile clean lint sc test format
.DEFAULT_GOAL = help

define HELP_LIST_TARGETS
To compile binary:
    $$ make compile
To remove build directory:
    $$ make clean
To run cppcheck linter:
    $$ make lint
To run shellcheck linter:
    $$ make sc
To run unit tests:
    $$ make test
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

sc:
	@shellcheck --shell=bash setup get_dependencies

test: compile
	@python3 -m pytest -vs tests
	@python3 -m pytest -vs tests --memory

format:
	@clang-format -i --verbose --style=file GPTifier/src/*.cpp GPTifier/include/*.hpp
