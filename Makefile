.PHONY = help compile clean lint test
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
endef

export HELP_LIST_TARGETS

help:
	@echo "$$HELP_LIST_TARGETS"

compile:
	@cmake -S src -B build
	@make --jobs=12 --directory=build install

clean:
	@rm -rfv build

lint:
	@cppcheck src/ --enable=all

test: compile
	@pytest -vs tests
