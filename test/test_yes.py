"""
Tests for `yes`.
"""

from helpers import check_version, run, run_with_timeout


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
    """`yes` normally prints stuff forever; we stop it after a timeout."""
    # Given that each repetition is "<phrase>\n", when we split on newlines,
    # the last line should be empty.

    lines = run_with_timeout(["yes"], timeout=0.25)[0].split("\n")
    assert all(line == "y" for line in lines[:-2])
    assert lines[-1] == ""

    lines = run_with_timeout(["yes", "2"], timeout=0.25)[0].split("\n")
    assert all(line == "2" for line in lines[:-2])
    assert lines[-1] == ""

    lines = run_with_timeout(["yes", "2", "3"], timeout=0.25)[0].split("\n")
    assert all(line == "2" for line in lines[:-2])
    assert lines[-1] == ""
