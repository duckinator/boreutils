from helpers import check, run

# https://pubs.opengroup.org/onlinepubs/9699919799/utilities/basename.html

def test_checkning_correct_program():
    assert check(["basename", "--version"]).stdout.split('(')[1].split(')')[0] == 'Boreutils'

def test_no_args():
    # No args => error of the form "basename: ..."
    assert run(["basename"]).stderr.startswith("basename:")
    assert run(["basename"]).returncode > 0

def test_help():
    # Passing -h or --help => print help text.
    assert run(["basename", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["basename", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["basename", "-h"]).returncode > 0
    assert run(["basename", "--help"]).returncode > 0

# Test the various steps:

def test_step1():
    # 1. Empty string results in an empty string.
    assert check(["basename", ""]).stdout == "\n"

def test_step2():
    # 2. We _do not_ skip steps 3-6 if given "//", so this should return "/".
    #    If we do skip step 3-6, this should return "//"!
    assert check(["basename", "//"]).stdout == "/\n"

def test_step3():
    # 3. If string is entirely slash characters, we get a single slash.
    assert check(["basename", "///"]).stdout == "/\n"

def test_step4():
    # 4. Remove trailing slash characters.
    assert check(["basename", "owo/"]).stdout == "owo\n"
    #! Potential edge case if we change behavior for step 2.
    assert check(["basename", "owo//"]).stdout == "owo\n"
    assert check(["basename", "owo///"]).stdout == "owo\n"

def test_step5():
    # 5. If there are remaining slash characters, remove everything up to and
    #    including the last slash.
    assert check(["basename", "/a/b/c/d/owo"]).stdout == "owo\n"
    #! Potential edge case exercising steps 4+5 together.
    assert check(["basename", "/a/b/c/d/owo///"]).stdout == "owo\n"
    assert check(["basename", "///a/b/c/d/owo///"]).stdout == "owo\n"

def test_step6():
    # 6. Remove suffix if it exists and is not the entire contents of the
    #    string.
    assert check(["basename","///a/b/owo.ext//", ".ext"]).stdout == "owo\n"
    assert check(["basename","///a/b/owo.ext2//", ".ext"]).stdout == "owo.ext2\n"
    assert check(["basename","///a/b/owo.ext", ".ext"]).stdout == "owo\n"
    assert check(["basename","///a/b/owo.ex", ".ext"]).stdout == "owo.ex\n"
