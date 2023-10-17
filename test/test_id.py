"""
Tests for POSIX-compatible `id`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/id.html
"""

import grp
import os
import pwd
from helpers import check_no_stderr, check_version, run

def get_group_ids(username):
    # Get list of all groups except the default one for this user.
    groups = [g.gr_gid for g in grp.getgrall() if username in g.gr_mem]
    # Add the default group for this user to the list.
    groups.append(pwd.getpwnam(username).pw_gid)
    return groups

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


def test_default_output():
    """Test output of just running `id` with no arguments."""
    uid = os.getuid()
    gid = os.getgid()
    username = pwd.getpwuid(uid).pw_name
    groupname = grp.getgrgid(gid).gr_name

    group_ids = get_group_ids(username)

    user, group, grouplist_str = check_no_stderr(["id", username]).stdout.split(" ", 2)

    assert user == f"uid={uid}({username})"
    assert group == f"gid={gid}({groupname})"
    assert grouplist_str.startswith("groups=")
    assert grouplist_str.endswith("\n")

    grouplist = grouplist_str.strip().split("=", 1)[1].split(",")
    for group_id in group_ids:
        group_name = grp.getgrgid(group_id).gr_name
        assert f"{group_id}({group_name})" in grouplist

    grouplist_str = check_no_stderr(["id", "-G", "-n", username]).stdout
    assert grouplist_str.endswith("\n")
    grouplist = grouplist_str.strip().split(" ")
    for group_id in group_ids:
        group_name = grp.getgrgid(group_id).gr_name
        assert group_name in grouplist


def test_main():
    """Test cases for actual functionality."""

    uid = os.getuid()
    gid = os.getgid()
    username = pwd.getpwuid(uid).pw_name

    assert check_no_stderr(["id", "-g", "-r", username]).stdout == str(gid) + "\n"
    assert check_no_stderr(["id", "-u", "-n", username]).stdout == username + "\n"
    assert check_no_stderr(["id", "-u", "-r", username]).stdout == str(uid) + "\n"
    assert check_no_stderr(["id", "-u", "-r", "-u", "-r", "-u", username]).stdout == str(uid) + "\n"
