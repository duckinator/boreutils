#!/usr/bin/env bash

fail() {
    echo "Failed at $2 for file: $("$1" | sed 's/-/ /')"
}

for x in $(cat test/test_cal.py | grep test_main -A10 | grep check | cut -d '[' -f 2 | cut -d ']' -f 1 | cut -d ' ' -f 2- | sed 's/,//' | sed 's/"//g' | head -n 6 | sed 's/ /-/'); do
    bash -c "./util/cal-diff.sh $(echo $x | sed 's/-/ /')" || fail "$x" "diff"
    FILENAME=$(echo $x | sed 's/\(.*\)-\(.*\)/\2-\1/')
    mv b test/test_cal/$FILENAME.txt || fail "$x" "mv"
done
