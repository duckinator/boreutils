"""
Tests for POSIX-compatible `kill`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/kill.html
"""

from helpers import check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("kill")


def test_missing_args():
    """No args => error of the form `kill: ...`"""
    assert run(["kill"]).stderr.startswith("kill:")
    assert run(["kill"]).returncode > 0


def test_extra_args():
    """Nothing to test: `kill` accepts any number args."""
    pass


def test_help():
    """Passing -h or --help should print help text."""
    assert run(["kill", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["kill", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["kill", "-h"]).returncode > 0
    assert run(["kill", "--help"]).returncode > 0


def test_main():
    """TODO"""
