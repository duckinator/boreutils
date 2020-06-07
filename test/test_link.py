"""
Tests for POSIX-compatible `link`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/link.html
"""

from pathlib import Path
from helpers import check, check_fail, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("link")


def test_missing_args():
    """No args => error of the form `link: ...`"""
    assert run(["link"]).stderr.startswith("link:")
    assert run(["link"]).returncode > 0


def test_extra_args():
    """Too many args => error of the form `link: ...`"""
    assert run(["link", "a", "b", "c"]).stderr.startswith("link:")
    assert run(["link", "a", "b", "c"]).returncode > 0


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["link", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["link", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["link", "-h"]).returncode > 0
    assert run(["link", "--help"]).returncode > 0


def test_main(tmpdir_factory):
    """Passing an existing filepath and non-existent filepath creates a link."""
    data = Path(tmpdir_factory.mktemp("data"))
    file1 = data / "file1.txt"
    file2 = data / "file2.txt"

    text = "hello, world!"
    file1.write_text(text)
    assert file1.read_text() == text
    assert not file2.exists()

    ret = check(["link", str(file1), str(file2)])
    assert ret.stdout == ""
    assert ret.stderr == ""

    assert file1.read_text() == text
    assert file2.read_text() == text


def test_file1_and_file2_do_not_exist(tmpdir_factory):
    """If the source filepath and destination filepath don't exist, raises an error."""
    data = Path(tmpdir_factory.mktemp("data"))
    file1 = data / "file1.txt"
    file2 = data / "file2.txt"

    assert not file1.exists()
    assert not file2.exists()

    ret = check_fail(["link", str(file1), str(file2)])
    assert ret.stdout == ""
    assert len(ret.stderr) > 0

    assert not file1.exists()
    assert not file2.exists()


def test_file1_does_not_exist(tmpdir_factory):
    """If the source filepath does not exist, raises an error."""
    data = Path(tmpdir_factory.mktemp("data"))
    file1 = data / "file1.txt"
    file2 = data / "file2.txt"

    file2_text = "this should not be overwritten"
    file2.write_text(file2_text)
    assert not file1.exists()
    assert file2.read_text() == file2_text

    ret = check_fail(["link", str(file1), str(file2)])
    assert ret.stdout == ""
    assert len(ret.stderr) > 0

    assert not file1.exists()
    assert file2.read_text() == file2_text


def test_file2_exists(tmpdir_factory):
    """If the destination filepath exists, raises an error."""
    data = Path(tmpdir_factory.mktemp("data"))
    file1 = data / "file1.txt"
    file2 = data / "file2.txt"

    file1_text = "hello, world!"
    file1.write_text(file1_text)
    file2_text = "this should not be overwritten"
    file2.write_text(file2_text)
    assert file1.read_text() == file1_text
    assert file2.read_text() == file2_text

    ret = check_fail(["link", str(file1), str(file2)])
    assert ret.stdout == ""
    assert len(ret.stderr) > 0

    assert file1.read_text() == file1_text
    assert file2.read_text() == file2_text
