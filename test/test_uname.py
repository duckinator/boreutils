"""
Tests for POSIX-compatible `uname`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/uname.html
"""

import os
from helpers import check, check_fail, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("uname")


def test_missing_args():
    """Nothing to check: uname accepts any number of arguments."""
    pass


def test_extra_args():
    """Nothing to check: uname accepts any number of arguments."""
    pass


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["uname", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["uname", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["uname", "-h"]).returncode > 0
    assert run(["uname", "--help"]).returncode > 0


def test_main():
    """Test that uname returns the correct values, and errors when appropriate."""
    uname = os.uname()
    uname_smr = f"{uname.sysname} {uname.release} {uname.machine}\n"
    uname_a = f"{uname.sysname} {uname.nodename} {uname.release} {uname.version} {uname.machine}\n"
    assert check(["uname"]).stdout == uname.sysname + "\n"
    assert check(["uname", "-m"]).stdout == uname.machine + "\n"
    assert check(["uname", "-n"]).stdout == uname.nodename + "\n"
    assert check(["uname", "-r"]).stdout == uname.release + "\n"
    assert check(["uname", "-s"]).stdout == uname.sysname + "\n"
    assert check(["uname", "-v"]).stdout == uname.version + "\n"
    assert check(["uname", "-smr"]).stdout == uname_smr
    assert check(["uname", "-mnrsv"]).stdout == uname_a
    assert check(["uname", "-a"]).stdout == uname_a
    assert check_fail(["uname", "-x"]).stderr.startswith("uname: Invalid")
    assert check_fail(["uname", "x"]).stderr.startswith("uname: Invalid")
