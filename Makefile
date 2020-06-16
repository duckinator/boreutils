CC := clang
CLANG_CHECK := clang-check

CFLAGS := -std=c11 -pedantic-errors -fdiagnostics-show-option \
			-Werror -Weverything -Wno-missing-noreturn -Wno-missing-braces \
			-D_XOPEN_SOURCE=700 -D_DEFAULT_SOURCE

SRCFILES := $(wildcard src/*.c)
EXEFILES := $(patsubst src/%,bin/%,$(patsubst %.c,%,${SRCFILES}))
RSTFILES := $(patsubst src/%.c,doc/%.rst,${SRCFILES})

all: ${EXEFILES} docs

docs: ${RSTFILES}

bin/%: src/%.c
	@mkdir -p bin/
	${CC} ${CFLAGS} $< -o $@

lint: compile_commands.json
	${CLANG_CHECK} -p . $(filter %.c,${SRCFILES}) $(wildcard src/%.h)

pylint:
	pylint test

test: all
	pytest

compile_commands.json:
	$(MAKE) clean
	bear $(MAKE) all

doc/%.rst: src/%.c
	@mkdir -p doc/
	./util/ccomex.py $< > $@

clean:
	rm -rf bin/
	rm -rf doc/

# compile_commands.json isn't _actually_ phony, but this ensures it's always built.
.PHONY: clean all compile_commands.json
