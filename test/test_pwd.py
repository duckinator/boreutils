"""
Tests for `pwd`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/pwd.html
"""

import os
from pathlib import Path
from helpers import check, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("pwd")


def test_missing_args():
    """Nothing to test: `pwd` doesn't require any arguments."""
    pass


def test_extra_args():
    """Extra args => error of the form `pwd: ...`"""
    assert run(["pwd", "owo"]).stderr.startswith("pwd: ")
    assert run(["pwd", "owo"]).returncode > 0


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["pwd", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["pwd", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["pwd", "-h"]).returncode > 0
    assert run(["pwd", "--help"]).returncode > 0


def test_main():
    """No args => user name associated with the current effective user ID."""
    assert check(["pwd"]).stdout == os.getcwd() + "\n"
    assert len(check(["pwd"]).stderr) == 0


def test_symlink_dash_p(tmpdir):
    """`pwd -P` should resolve symlinks before returning the directory."""
    a = Path(tmpdir) / 'a'
    b = Path(tmpdir) / 'b'

    a.mkdir()
    b.symlink_to(a, target_is_directory=True)

    assert run(["pwd", "-P"], cwd=b).stdout == str(a) + "\n"
