"""
Tests for POSIX-compatible `echo`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/echo.html
"""

from helpers import check, check_version


def test_version():
    """Check that we're using Boreutil's implementation."""
    # Can't test echo directly, since it ignores --version.
    assert check_version("cat")
    assert check(["echo", "--version"]).stdout == "--version\n"


def test_missing_args():
    """Nothing to test: `echo` accepts any number of arguments."""
    pass


def test_extra_args():
    """Nothing to test: `echo` accepts any number of arguments."""
    pass


def test_help():
    """`echo` should always print its arguments. Even -h or --help."""
    assert check(["echo", "--help"]).stdout == "--help\n"
    assert check(["echo", "-h"]).stdout == "-h\n"
    assert check(["echo", "-n"]).stdout == "-n\n"


def test_main():
    """Echo should print all arguments, no matter what."""
    assert check(["echo"]).stdout == "\n"
    assert check(["echo", "owo"]).stdout == "owo\n"
    assert check(["echo", "owo\\nuwu"]).stdout == "owo\\nuwu\n"
