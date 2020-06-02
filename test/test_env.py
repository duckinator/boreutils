"""
Tests for POSIX-compatible `env`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/env.html
"""

from helpers import check, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("env")


def test_missing_args():
    """Nothing to test: `env` accepts any number of arguments."""
    pass


def test_extra_args():
    """Nothing to test: `env` accepts any number of arguments."""
    pass


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["env", "-h"]).stdout.split(" ")[0] == "Usage:"
    assert run(["env", "--help"]).stdout.split(" ")[0] == "Usage:"
    assert run(["env", "-h"]).returncode > 0
    assert run(["env", "--help"]).returncode > 0


def test_main():
    """This was split into the test_step[1-8] functions below."""
    pass


def test_exec():
    """Test that specifying a utility will run it."""
    assert check(["env", "echo"]).stdout == "\n"
    assert check(["env", "echo", "hi"]).stdout == "hi\n"
    assert check(["env", "echo", "hello,", "world!"]).stdout == "hello, world!\n"


def test_print():
    assert check(["env"], env={"a": "b", "FOO": "BAR"}).stdout == "a=b\nFOO=BAR\n"


def test_dash_i():
    assert check(["env", "-i", "a=b", "c=d", "E=F G H I"]).stdout == "a=b\nc=d\nE=F G H I\n"
    # NOTE: This next test may be fragile, since it relies on a functioning `sh`.
    assert check(["env", "-i", "foo=bar", "sh", "-c", "echo ${foo}"]).stdout == "bar\n"
