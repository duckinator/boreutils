CC := clang

CFLAGS := -std=c11 -pedantic-errors -fdiagnostics-show-option \
			-Werror -Weverything

SRCFILES := $(wildcard src/*.c)
EXEFILES := $(patsubst src/%,bin/%,$(patsubst %.c,%,${SRCFILES}))

all: ${EXEFILES}

bin/%: src/%.c
	@mkdir -p bin/
	${CC} ${CFLAGS} $< -o $@

test: all
	pytest

clean:
	rm -rf bin/

.PHONY: clean all
