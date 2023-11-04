.PHONY = help compile clean lint
.DEFAULT_GOAL = help

define HELP_LIST_TARGETS
To compile binary:
    $$ make compile
To remove build directory:
    $$ make clean
To run cppcheck linter:
    $$ make lint
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
