"""
Tests for POSIX-compatible `true`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/true.html
"""

from helpers import check_version, run


def test_checking_correct_program():
    """Check that we're using Boreutil's implementation."""
    assert check_version("true")


def test_normal_usage():
    """Running `true` with out -h/--help/--version should print nothing and
       exit with return code of 0."""
    assert len(run(["true"]).stdout) == 0
    assert len(run(["true"]).stderr) == 0
    assert run(["true"]).returncode == 0


def test_help():
    """Passing -h or --help should print help text."""
    assert run(["true", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["true", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["true", "-h"]).returncode > 0
    assert run(["true", "--help"]).returncode > 0