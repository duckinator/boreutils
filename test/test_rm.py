"""
Tests for POSIX-compatible `rm`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/rm.html
"""

from pathlib import Path
import pytest
from helpers import check, check_fail, check_version, run


@pytest.fixture()
def tree(tmpdir_factory):
    data = tmpdir_factory.mktemp("data")
    data_p = Path(data) / "test1" / "test2"
    data_p.mkdir(parents=True)
    a = data_p / "a.txt"
    b = data_p / "b.txt"
    a.write_text("a")
    b.write_text("b")
    assert a.read_text() == "a"
    assert b.read_text() == "b"
    return (data_p, a, b)


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("rm")


def test_missing_args():
    """`rm` requires at least one argument -- either -f or a file."""
    pass


def test_extra_args():
    """Nothing to test: `rm` accepts 1 or more arguments."""
    pass


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["rm", "-h"]).stdout.split(" ")[0] == "Usage:"
    assert run(["rm", "--help"]).stdout.split(" ")[0] == "Usage:"
    assert run(["rm", "-h"]).returncode > 0
    assert run(["rm", "--help"]).returncode > 0


def test_main():
    """This was split into multiple functions below."""
    pass


def test_no_flags(tree):
    """Test without -f, -i, or -r/-R."""
    data, a, b = tree
    assert "directory" in check_fail(["rm", str(data)]).stderr
    assert data.exists()
    assert a.exists()
    assert b.exists()

    ret = check(["rm", str(a)])
    assert len(ret.stdout) == 0
    assert len(ret.stderr) == 0


def test_r(tree):
    """Test with -r."""
    data, a, b = tree
    ret = check(["rm", "-r", str(data)])
    assert len(ret.stdout) == 0
    assert len(ret.stderr) == 0
    assert not data.exists()
    assert not a.exists()
    assert not b.exists()


def test_r__file(tree):
    """Test with -r."""
    data, a, b = tree
    ret = check(["rm", "-r", str(a)])
    assert len(ret.stdout) == 0
    assert len(ret.stderr) == 0
    assert data.exists()
    assert not a.exists()
    assert b.exists()


def test_f(tree):
    """Test with -f."""
    data, a, b = tree
    ret = check(["rm", "-f", str(data)])
    assert len(ret.stdout) == 0
    assert len(ret.stderr) == 0
    assert data.exists()
    assert a.exists()
    assert b.exists()

    ret = check(["rm", "-f", str(a)])
    assert len(ret.stdout) == 0
    assert len(ret.stderr) == 0


def test_if(tree):
    """Test with -f."""
    data, a, b = tree
    ret = check(["rm", "-if", str(data)])
    assert len(ret.stdout) == 0
    assert len(ret.stderr) == 0
    assert data.exists()
    assert a.exists()
    assert b.exists()

    ret = check(["rm", "-i", "-f", str(data)])
    assert len(ret.stdout) == 0
    assert len(ret.stderr) == 0
    assert data.exists()
    assert a.exists()
    assert b.exists()

    ret = check(["rm", "-if", str(a)])
    assert len(ret.stdout) == 0
    assert len(ret.stderr) == 0


def test_i(tree):
    """Test with -i, all negative responses."""
    data, a, b = tree
    ret = check(["rm", "-i", str(a)], input="n\n")
    assert len(ret.stdout) == 0
    assert len(ret.stderr) > 0
    assert data.exists()
    assert a.exists()
    assert b.exists()

    ret = check(["rm", "-ri", str(a), str(b), str(data)], input="n\n")
    assert len(ret.stdout) == 0
    assert len(ret.stderr) > 0
    assert data.exists()
    assert a.exists()
    assert b.exists()


def test_i2(tree):
    """Test with -i, mixed responses."""
    data, a, b = tree
    ret = check(["rm", "-i", str(a), str(b)], input="y\nn\n")
    assert len(ret.stdout) == 0
    assert len(ret.stderr) > 0
    assert data.exists()
    assert not a.exists()
    assert b.exists()


def test_i3(tree):
    """Test with -i, mixed responses."""
    data, a, b = tree
    ret = check(["rm", "-f", "-i", str(data), str(b)], input="n\ny\n")
    assert len(ret.stdout) == 0
    assert len(ret.stderr) > 0
    assert data.exists()
    assert a.exists()
    assert not b.exists()
