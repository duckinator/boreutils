"""
Tests for POSIX-compatible `id`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/id.html
"""

import grp
import os
import pwd
from helpers import check_no_stderr, check_version, run

def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("id")


def test_missing_args():
    """Nothing to test: `id` accepts 0 or more arguments.."""
    pass


def test_extra_args():
    """Nothing to test: `id` accepts 0 or more arguments."""
    pass


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["id", "-h"]).stdout.split(" ")[0] == "Usage:"
    assert run(["id", "--help"]).stdout.split(" ")[0] == "Usage:"
    assert run(["id", "-h"]).returncode > 0
    assert run(["id", "--help"]).returncode > 0


def test_main():
    """Test cases for actual functionality."""

    uid = os.getuid()
    gid = os.getgid()
    username = pwd.getpwuid(uid).pw_name
    groupname = grp.getgrgid(gid).gr_name

    #group_ids = []

    #id_parts = [
    #    "uid={}({})".format(uid, username),
    #    "gid{}({})".format(gid, groupname),
    #]
    id_str = "uid={}({}) gid={}({}) ".format(uid, username, gid, groupname)
    assert check_no_stderr(["id", username]).stdout.startswith(id_str)

    assert groupname in check_no_stderr(["id", "-G", "-n", username]).stdout
    assert check_no_stderr(["id", "-g", "-r", username]).stdout == str(gid) + "\n"
    assert check_no_stderr(["id", "-u", "-n", username]).stdout == username + "\n"
    assert check_no_stderr(["id", "-u", "-r", username]).stdout == str(uid) + "\n"
    assert check_no_stderr(["id", "-u", "-r", "-u", "-r", "-u", username]).stdout == str(uid) + "\n"
