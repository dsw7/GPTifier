.PHONY = help compile clean
.DEFAULT_GOAL = help

define HELP_LIST_TARGETS
To compile binary:
    $$ make compile
To remove build directory:
    $$ make clean
endef

export HELP_LIST_TARGETS

help:
	@echo "$$HELP_LIST_TARGETS"

compile:
	@cmake -S src -B build
	@make --jobs=12 --directory=build

clean:
	@rm --recursive --verbose build
