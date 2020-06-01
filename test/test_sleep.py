"""
Tests for POSIX-compatible `sleep`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/sleep.html
"""

import time
from helpers import check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("sleep")


def test_missing_args():
    """No args => error of the form `sleep: ...`"""
    assert run(["sleep"]).stderr.startswith("sleep:")
    assert run(["sleep"]).returncode > 0


def test_extra_args():
    """Too many args => error of the form `sleep: ...`"""
    assert run(["sleep", "a", "b"]).stderr.startswith("sleep:")
    assert run(["sleep", "a", "b"]).returncode > 0


def test_help():
    """Passing -h or --help should print help text."""
    assert run(["sleep", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["sleep", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["sleep", "-h"]).returncode > 0
    assert run(["sleep", "--help"]).returncode > 0


def test_main():
    """Running `sleep 1` should run successfully."""
    start = time.time()
    ret = run(["sleep", "2"])
    end = time.time()

    assert len(ret.stdout) == 0
    assert len(ret.stderr) == 0
    assert ret.returncode == 0
    assert (end - start) >= 2.0
