"""
Tests for POSIX-compatible `date`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/date.html
"""

import time
from helpers import check_version, check, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("date")


def test_missing_args():
    """Nothing to test: `date` accepts any number of arguments."""
    pass


def test_extra_args():
    """Nothing to test: `date` accepts any number of arguments."""
    pass


def test_help():
    """Passing -h or --help should print help text."""
    assert run(["date", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["date", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["date", "-h"]).returncode > 0
    assert run(["date", "--help"]).returncode > 0


def test_main():
    """Running `date` should print the current date, using the appropriate
       format string.

       TODO: The same tests, but UTC (with -u)."""

    # Default format string.
    assert check(["date"]).stdout == time.strftime("%a %b %e %H:%M:%S %Z %Y\n")
    assert len(check(["date"]).stderr) == 0

    # Format string for ISO 8601.
    assert check(["date", "+%FT%TZ"]).stdout == time.strftime("%FT%TZ\n")
    assert len(check(["date", "+%FT%TZ"]).stderr) == 0
