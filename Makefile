.POSIX:

# Some potential options:
# - SANITIZE=address,undefined
# - SANITIZE=memory

CC := clang
CLANG_CHECK := clang-check

CFLAGS := -std=c11 -g -pedantic-errors -fsanitize=${SANITIZE} \
	-Wall -Wextra -Wconversion -Wcast-qual \
	-D_XOPEN_SOURCE=700

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
	pylint $$(find test -name '*.py')

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
