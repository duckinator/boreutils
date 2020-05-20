"""
Tests for `whoami`.

TODO: Determine if there's a standard for this.
"""

import os
from helpers import check, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("whoami")


def test_missing_args():
    """Nothing to test: `whoami` doesn't require any arguments."""
    pass


def test_extra_args():
    """Extra args => error of the form `whoami: ...`"""
    assert run(["whoami", "owo"]).stderr.startswith("whoami: ")
    assert run(["whoami", "owo"]).returncode > 0


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["whoami", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["whoami", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["whoami", "-h"]).returncode > 0
    assert run(["whoami", "--help"]).returncode > 0


def test_main():
    """No args => user name associated with the current effective user ID."""
    assert check(["whoami"]).stdout == os.getlogin() + "\n"
    assert len(check(["whoami"]).stderr) == 0
