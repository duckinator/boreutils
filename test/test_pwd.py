"""
Tests for `pwd`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/pwd.html
"""

import os
from helpers import check, check_version, run


def test_checking_correct_program():
    """Check that we're using Boreutil's implementation."""
    assert check_version("pwd")


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

def test_dash_p():
    # TODO: Create a symlink to a directory, cd to the symlink, and check
    #       bin/pwd output doesn't include it.
    pass
