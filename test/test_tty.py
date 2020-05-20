"""
Tests for `tty`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/tty.html
"""

import os
from helpers import check, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("tty")


def test_missing_args():
    """Nothing to test: `tty` doesn't require any arguments."""
    pass


def test_extra_args():
    """Extra args => error of the form `tty: ...`"""
    assert run(["tty", "owo"]).stderr.startswith("tty: ")
    assert run(["tty", "owo"]).returncode > 0


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["tty", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["tty", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["tty", "-h"]).returncode > 0
    assert run(["tty", "--help"]).returncode > 0


def test_main():
    """I haven't found a way to automate these tests because they require a tty."""
    # `tty`/`tty -s` should return 0, if ran from a TTY.
    # assert check(["tty"]).returncode == 0
    # assert run(["tty", "-s"]).stdout == 0

    # `tty` should print a tty name, if ran from a TTY.
    # assert check_tty(["tty"]).stdout == os.ttyname(0) + "\n"

    # `tty -s` should print nothing, always.
    assert run(["tty", "-s"]).stdout == ""
    # assert check_tty(["tty", "-s"]).stdout == ""

    # `tty -s` should return >0, if not ran from a TTY.
    assert run(["tty", "-s"]).returncode > 0
