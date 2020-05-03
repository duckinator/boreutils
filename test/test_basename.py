from util import run, fails
import os

os.environ['PATH'] += ':./bin'

# https://pubs.opengroup.org/onlinepubs/9699919799/utilities/basename.html
basename = "./bin/basename"

def test_running_correct_program():
    assert run(["basename", "--version"]).stdout.split('(')[1].split(')')[0] == 'Boreutils'

def test_no_args():
    # No args => error of the form "basename: ..." or "/path/to/basename: ..."
    assert fails(["basename"]).stderr.split(' ')[0].endswith("basename:")

def test_help():
    # Passing -h or --help => print help text.
    assert fails(["basename", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert fails(["basename", "--help"]).stdout.split(' ')[0] == 'Usage:'

# Test the various steps:

def test_step1():
    # 1. Empty string results in an empty string.
    assert run(["basename", ""]).stdout == "\n"

def test_step2():
    # 2. We _do not_ skip steps 3-6 if given "//", so this should return "/".
    #    If we do skip step 3-6, this should return "//"!
    assert run(["basename", "//"]).stdout == "/\n"

def test_step3():
    # 3. If string is entirely slash characters, we get a single slash.
    assert run(["basename", "///"]).stdout == "/\n"

def test_step4():
    # 4. Remove trailing slash characters.
    assert run(["basename", "owo/"]).stdout == "owo\n"
    #! Potential edge case if we change behavior for step 2.
    assert run(["basename", "owo//"]).stdout == "owo\n"
    assert run(["basename", "owo///"]).stdout == "owo\n"

def test_step5():
    # 5. If there are remaining slash characters, remove everything up to and
    #    including the last slash.
    assert run(["basename", "/a/b/c/d/owo"]).stdout == "owo\n"
    #! Potential edge case exercising steps 4+5 together.
    assert run(["basename", "/a/b/c/d/owo///"]).stdout == "owo\n"
    assert run(["basename", "///a/b/c/d/owo///"]).stdout == "owo\n"
