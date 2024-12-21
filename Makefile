.PHONY = help format compile clean lint test
.DEFAULT_GOAL = help

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
endef

export HELP_LIST_TARGETS

help:
	@echo "$$HELP_LIST_TARGETS"

format:
	@clang-format -i --verbose --style=file GPTifier/src/*.cpp GPTifier/include/*.hpp

compile: format
	@cmake -S GPTifier -B build
	@make --jobs=12 --directory=build install

clean:
	@rm -rfv build

lint:
	@cppcheck GPTifier --enable=all

test: compile
	@python3 -m pytest -v tests
	@python3 -m pytest -v tests --memory
