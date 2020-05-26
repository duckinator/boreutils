#!/usr/bin/env bash

echo "There are no tests for:"

for filename in src/*.c; do
    util=$(echo $filename | cut -d '/' -f 2 | cut -d '.' -f 1)
    if ! stat test/test_$util.py &>/dev/null; then
        echo "- $util"
    fi
done

echo
echo

echo "Common tests which are missing:"

check() {
    { grep "def $2()" "$1" &>/dev/null; } || echo "- $1: $2"
}

for f in test/test_*.py; do
    check "$f" "test_version"
    check "$f" "test_missing_args"
    check "$f" "test_extra_args"
    check "$f" "test_help"
    check "$f" "test_main"
done
