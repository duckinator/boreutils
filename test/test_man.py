"""
Tests for the worlds crappiest `man` implementation.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/man.html
"""

from pathlib import Path
from helpers import check, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("man")


def test_missing_args():
    """Passing no args should return a 'missing argument' message."""
    assert run(["man"]).stderr.startswith("man: Missing argument\n")


def test_extra_args():
    """Nothing to test: `man` accepts any number of arguments."""
    pass


def test_help():
    """Passing -h or --help should print help text."""
    assert run(["man", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["man", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["man", "-h"]).returncode > 0
    assert run(["man", "--help"]).returncode > 0


def test_main():
    """Running `man PAGE` should print the contents of that file"""
    pages = ["basename", "cal", "cat", "date", "dirname", "echo", "false",
             "ish", "man", "pwd", "true", "tty", "whoami", "yes"]
    for page in pages:
        assert check(["man", page]).stdout == (Path("doc") / page).with_suffix(".rst").read_text()
