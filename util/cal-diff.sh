#!/usr/bin/env bash

if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    echo "Usage: $0 <args for \`cal\`>"
    exit 1
fi

echo "cal $@"
make clean bin/cal >/dev/null

if [ -n "$1" ] && [ -z "$2" ]; then
    # This assumes /bin/cal is or behaves like GNU cal.
    # `bin/cal <year>` behaves close to GNU cal called in a loop
    for i in `seq 1 12`; do
        /bin/cal $i "$1" | sed 's/^ \+$//';
    done > a || exit 1

    ./bin/cal "$1" > b || exit 1
else
    /bin/cal "$1" "$2" > a
    ./bin/cal "$1" "$2" > b
fi

diff -B -Z a b
