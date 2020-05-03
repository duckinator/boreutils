from .util import prints_help, run, fails
from pathlib import Path

# https://pubs.opengroup.org/onlinepubs/9699919799/utilities/basename.html

basename = "./bin/basename"

assert run([basename, "--version"]).stdout.decode().split(' ')[0] == 'Boreutils'

# No args => error of the form "basename: ..." or "/path/to/basename: ..."
assert fails([basename]).stderr.decode().split(' ')[0].endswith("basename:")

# Test the various steps:

## 1. Empty string results in an empty string.
assert run([basename, ""]).stdout.decode().strip() == ""

## 2. We _do not_ skip steps 3-6 if given "//", so this should return "/".
##    If we do skip step 3-6, this should return "//"!
assert run([basename, "//"]).stdout.decode().strip() == "/"

## 3. If string is entirely slash characters, we get a single slash.
assert run([basename, "///"]).stdout.decode().strip() == "/"

## 4. Remove trailing slash characters.
assert run([basename, "owo/"]).stdout.decode().strip() == "owo"
#! Potential edge case if we change behavior for step 2.
assert run([basename, "owo//"]).stdout.decode().strip() == "owo"
assert run([basename, "owo///"]).stdout.decode().strip() == "owo"

## 5. If there are remaining slash characters, remove everything up to and
##    including the last slash.
assert run([basename, "/a/b/c/d/owo"]).stdout.decode().strip() == "owo"
#! Potential edge case exercising steps 4+5 together.
assert run([basename, "/a/b/c/d/owo///"]).stdout.decode().strip() == "owo"
assert run([basename, "///a/b/c/d/owo///"]).stdout.decode().strip() == "owo"
