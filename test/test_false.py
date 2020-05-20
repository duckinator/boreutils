"""
Tests for POSIX-compatible `false`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/false.html
"""

from helpers import check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("false")


def test_normal_usage():
    """Running `false` with out -h/--help/--version should print nothing and
       exit with return code of 1."""
    assert len(run(["false"]).stdout) == 0
    assert len(run(["false"]).stderr) == 0
    assert run(["false"]).returncode == 1


def test_help():
    """Passing -h or --help should print help text."""
    assert run(["false", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["false", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["false", "-h"]).returncode > 0
    assert run(["false", "--help"]).returncode > 0
