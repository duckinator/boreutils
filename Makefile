.POSIX:

# Some potential options:
# - SANITIZE=address,undefined
# - SANITIZE=memory

CC := clang
CLANG_CHECK := clang-check

CFLAGS := -std=c11 -pedantic-errors -fdiagnostics-show-option \
			-Werror -Weverything -Wno-missing-noreturn -Wno-unused-macros \
			-D_XOPEN_SOURCE=700 -D_DEFAULT_SOURCE \
			-fsanitize=${SANITIZE} -g

SRCFILES != ls src/*.c
EXEFILES != echo ${SRCFILES} | sed 's/src/bin/g' | sed 's/\.c//g'
RSTFILES != echo ${SRCFILES} | sed 's/src/doc/g' | sed 's/\.c/.rst/g'

all: ${EXEFILES} docs

docs: ${RSTFILES}

${EXEFILES}:
	@mkdir -p bin/
	${CC} ${CFLAGS} $$(echo $@ | sed 's/bin\//src\//').c -o $@

lint: compile_commands.json
	${CLANG_CHECK} -p . ${SRCFILES}

pylint:
	pylint test

test: all
	pytest

compile_commands.json:
	$(MAKE) clean
	bear $(MAKE) all

${RSTFILES}:
	@mkdir -p doc/
	./util/ccomex.py $$(echo $@ | sed 's/doc\//src\//' | sed 's/\.rst/\.c/') > $@

clean:
	rm -rf bin/
	rm -rf doc/

# compile_commands.json isn't _actually_ phony, but this ensures it's always built.
.PHONY: clean all compile_commands.json
