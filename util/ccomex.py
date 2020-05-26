#!/usr/bin/env python3

from pathlib import Path
import sys


def extract(filename):
    text = ""
    lines = Path(filename).read_text().splitlines()

    capture = False
    for line in lines:
        if line.strip() == '/**':
            capture = True
            continue
        if line.strip() == '*/':
            capture = False
            break
        if not capture:
            continue

        # If we get to this point, we're in the first doc comment.

        if line.strip() == '*':
            text += "\n"
            continue

        if line[0] == ' ':
            line = line[1:]

        if line[0] == '*':
            line = line[1:]

        if line[0] == ' ':
            line = line[1:]

        text += line + "\n"
    return text


print(extract(sys.argv[1]))
