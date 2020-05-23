CC := clang

CFLAGS := -std=c11 -pedantic-errors -fdiagnostics-show-option \
			-Werror -Weverything -D_XOPEN_SOURCE=700

SRCFILES := $(wildcard src/*.c)
EXEFILES := $(patsubst src/%,bin/%,$(patsubst %.c,%,${SRCFILES}))

all: ${EXEFILES}

bin/%: src/%.c
	@mkdir -p bin/
	${CC} ${CFLAGS} $< -o $@

test: all
	pytest

compile_commands.json:
	$(MAKE) clean
	bear $(MAKE) all

clean:
	rm -rf bin/

# compile_commands.json isn't _actually_ phony, but this ensures it's always built.
.PHONY: clean all compile_commands.json
