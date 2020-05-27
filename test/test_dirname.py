"""
Tests for POSIX-compatible `dirname`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/dirname.html
"""

from helpers import check, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("dirname")


def test_missing_args():
    """No args => error of the form `dirname: ...`"""
    assert run(["dirname"]).stderr.startswith("dirname:")
    assert run(["dirname"]).returncode > 0


def test_extra_args():
    """Too many args => error of the form `dirname: ...`"""
    assert run(["dirname", "a", "b", "c"]).stderr.startswith("dirname:")
    assert run(["dirname", "a", "b", "c"]).returncode > 0


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["dirname", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["dirname", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["dirname", "-h"]).returncode > 0
    assert run(["dirname", "--help"]).returncode > 0


def test_main():
    """This was split into the test_step[1-8] functions below."""
    pass


# Test the various steps:


def test_step1():
    """1. If string is "//", return a slash."""
    assert check(["dirname", "//"]).stdout == "/\n"


def test_step2():
    """2. If string consists entirely of slashes, return a single slash."""
    assert check(["dirname", "////////////////////////////"]).stdout == "/\n"


def test_step3():
    """3. Remove trailing slashes."""
    assert check(["dirname", "/foo//////"]).stdout == "/\n"
    assert check(["dirname", "/foo/bar//////"]).stdout == "/foo\n"


def test_step4():
    """4. If there are no slashes, return a dot."""
    assert check(["dirname", "owo"]).stdout == ".\n"
    assert check(["dirname", "."]).stdout == ".\n"


def test_step5():
    """5. If there are any trailing non-<slash> characters, remove them."""
    assert check(["dirname", "/owo/uwu"]).stdout == "/owo\n"


def test_step6():
    """6. If the remaining string is "//", it is implementation defined
       whether to skip the remaining steps. We chose _not_ to skip them."""
    assert check(["dirname", "//fuck"]).stdout == "/\n"


def test_step7():
    """7. Remove trailing slashes again."""
    assert check(["dirname", "/foo////////bar"]).stdout == "/foo\n"


def test_step8():
    """8. If the remaining string is empty, return a slash."""
    assert check(["dirname", "///"]).stdout == "/\n"
