"""
Tests for `which`.

This utilitiy is not defined by the POSIX standard.
"""

from pathlib import Path
from helpers import check_version, check, check_fail, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("which")


def test_missing_args():
    """Passing no args should return a 'missing argument' message."""
    assert run(["which"]).stderr.startswith("which: Missing argument\n")



def test_extra_args():
    """Nothing to test: `which` accepts any number of arguments."""
    pass


def test_help():
    """Passing -h or --help should print help text."""
    assert run(["which", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["which", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["which", "-h"]).returncode > 0
    assert run(["which", "--help"]).returncode > 0


def test_nonexistent():
    """Running `which [-as] <program>` where <program> does not exist
       should exit with a nonzero status."""
    assert len(check_fail(["which", "this-program-does-not-exist"]).stdout) == 0
    assert len(check_fail(["which", "-a", "this-program-does-not-exist"]).stdout) == 0
    assert len(check_fail(["which", "-s", "this-program-does-not-exist"]).stdout) == 0
    assert len(check_fail(["which", "-as", "this-program-does-not-exist"]).stdout) == 0
    assert len(check_fail(["which", "-sa", "this-program-does-not-exist"]).stdout) == 0


def test_main():
    """Running `which <program>` should show the first instance of <program>.
       Adding -a should show all matching executables.
       Adding -s should return 0 if all <program>s were found, 1 otherwise."""

    # One program, no -a, no -s.
    result = check(["which", "dir"])
    assert result.stdout == str(Path("bin/dir").resolve()) + "\n"
    assert len(result.stderr) == 0

    # One program, -a, no -s.
    result = check(["which", "-a", "env"])
    lines = result.stdout.split("\n")
    assert str(Path("bin/env").resolve()) in lines
    assert "/usr/bin/env" in lines

    # One program, no -a, -s.
    result = check(["which", "-s", "env"])
    assert len(result.stdout) == 0
    assert len(result.stderr) == 0

    # One program, -a, -s.
    result = check(["which", "-a", "-s", "env"])
    assert len(result.stdout) == 0
    assert len(result.stderr) == 0

    # One program, -as
    result = check(["which", "-as", "env"])
    assert len(result.stdout) == 0
    assert len(result.stderr) == 0

    # One program, -sa
    result = check(["which", "-sa", "env"])
    assert len(result.stdout) == 0
    assert len(result.stderr) == 0

    # Four programs, no -a, no -s.
    result = check(["which", "dir", "cat", "echo", "which"])
    lines = result.stdout.split("\n")
    assert str(Path("bin/dir").resolve()) in lines
    assert str(Path("bin/cat").resolve()) in lines
    assert str(Path("bin/echo").resolve()) in lines
    assert str(Path("bin/which").resolve()) in lines
    assert len(result.stderr) == 0
