"""
Tests for POSIX-compatible `mkdir`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/mkdir.html
"""

from pathlib import Path
from helpers import chdir, check, check_fail, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("mkdir")


def test_missing_args():
    """No args => error of the form `mkdir: ...`"""
    assert check_fail(["mkdir"]).stderr.startswith("mkdir:")


def test_extra_args():
    """Passing more than one directory should raise an error."""
    assert check_fail(["mkdir", "a", "b"]).stderr.startswith("mkdir:")
    assert check_fail(["mkdir", "-p", "a", "b"]).stderr.startswith("mkdir:")
    assert check_fail(["mkdir", "-m", "777", "a", "b"]).stderr.startswith("mkdir:")


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["mkdir", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["mkdir", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["mkdir", "-h"]).returncode > 0
    assert run(["mkdir", "--help"]).returncode > 0


def test_main():
    """Split between test_absolute_paths() and test_relative_paths()."""
    pass


def test_absolute_paths(tmpdir_factory):
    """mkdir can create absolute paths"""
    data = Path(tmpdir_factory.mktemp("data"))
    a = data / "a"
    b = data / "b" / "b2"
    # c = data / "c"

    assert not a.exists()
    assert check(["mkdir", str(a)])
    assert a.is_dir()

    assert not b.exists()
    assert check(["mkdir", "-p", str(b)])
    assert b.is_dir()

    # assert not c.exists()
    # assert check(["mkdir", "-m", "777", str(c)])
    # assert c.is_dir()


def test_relative_paths(tmpdir_factory):
    """mkdir can create relative paths."""
    data = Path(tmpdir_factory.mktemp("data"))
    a = data / "a"
    b = data / "b" / "b2"
    # c = data / "c"

    with chdir(data):
        assert not a.exists()
        assert check(["mkdir", "a"])
        assert a.is_dir()

        assert not b.exists()
        assert check(["mkdir", "-p", "b/b2"])
        assert b.is_dir()

        # assert not c.exists()
        # assert check(["mkdir", "-m", "777", "c"])
        # assert c.is_dir()
