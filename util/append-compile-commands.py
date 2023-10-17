#!/usr/bin/env python3

# I couldn't get Bear to behave, so I made this instead.

from pathlib import Path
import json
import sys

cc = Path("compile_commands.json")

if cc.exists():
    with cc.open() as f:
        commands = json.load(f)
else:
    commands = []

current = {
    "arguments": sys.argv[1:],
    "directory": str(Path.cwd()),
    # current["file"] is the argumment after "-o".
    "file": sys.argv[sys.argv.index("-o") + 1],
    # current["output"] is the argument that has "bin/" in it.
    "output": next(filter(lambda x: "bin/" in x, sys.argv)),
}

commands.append(current)

with cc.open("w") as f:
    json.dump(commands, f, indent=2)
