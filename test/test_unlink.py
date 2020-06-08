"""
Tests for POSIX-compatible `unlink`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/unlink.html
"""

from pathlib import Path
from helpers import check, check_fail, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("unlink")


def test_missing_args():
    """No args => error of the form `unlink: ...`"""
    assert run(["unlink"]).stderr.startswith("unlink:")
    assert run(["unlink"]).returncode > 0


def test_extra_args():
    """Too many args => error of the form `unlink: ...`"""
    assert run(["unlink", "a", "b"]).stderr.startswith("unlink:")
    assert run(["unlink", "a", "b"]).returncode > 0


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["unlink", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["unlink", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["unlink", "-h"]).returncode > 0
    assert run(["unlink", "--help"]).returncode > 0


def test_main(tmpdir_factory):
    """Passing an existing filepath removes it."""
    data = Path(tmpdir_factory.mktemp("data"))
    path = data / "file1.txt"

    text = "hello, world!"
    path.write_text(text)
    assert path.read_text() == text

    ret = check(["unlink", str(path)])
    assert ret.stdout == ""
    assert ret.stderr == ""

    assert not path.exists()


def test_nonexistent_filepath(tmpdir_factory):
    """Passing a nonexistent filepath reports an error."""
    data = Path(tmpdir_factory.mktemp("data"))
    path = data / "file1.txt"
    assert not path.exists()

    ret = check_fail(["unlink", str(path)])
    assert ret.stdout == ""
    assert len(ret.stderr) > 0

    assert not path.exists()
