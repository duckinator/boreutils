"""
Tests for `yes`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/yes.html
"""

from helpers import check, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("yes")


def test_missing_args():
    """Nothing to test: `yes` accepts any number args."""
    pass


def test_extra_args():
    """Nothing to test: `yes` accepts any number args."""
    pass


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["yes", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["yes", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["yes", "-h"]).returncode > 0
    assert run(["yes", "--help"]).returncode > 0


def test_main():
    """`yes` normally prints stuff forever; the tests exit after 3 iterations."""
    assert check(["yes", "-Wtesting"]).stdout == "y\ny\ny\n"
    assert check(["yes", "-Wtesting", "2", "3"]).stdout == "-Wtesting 2 3\n-Wtesting 2 3\n-Wtesting 2 3\n"
