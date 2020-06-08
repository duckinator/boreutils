"""
Tests for POSIX-compatible `rmdir`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/rmdir.html
"""

from pathlib import Path
from helpers import chdir, check, check_fail, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("rmdir")


def test_missing_args():
    """No args => error of the form `rmdir: ...`"""
    assert run(["rmdir"]).stderr.startswith("rmdir:")
    assert run(["rmdir"]).returncode > 0


def test_extra_args():
    """Nothing to test: rmdir accepts 1+ arguments."""
    pass


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["rmdir", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["rmdir", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["rmdir", "-h"]).returncode > 0
    assert run(["rmdir", "--help"]).returncode > 0


def test_main(tmpdir_factory):
    """Passing an existing, empty directory removes it."""
    data = Path(tmpdir_factory.mktemp("data"))
    dir1 = data / "dir1"
    dir2 = data / "dir2"
    dir3 = data / "dir3"

    dir1.mkdir()
    dir2.mkdir()
    dir3.mkdir()

    assert dir1.is_dir()
    assert dir2.is_dir()
    assert dir3.is_dir()

    with chdir(data):
        ret = check(["rmdir", str(dir1), str(dir2)])
    assert ret.stdout == ""
    assert ret.stderr == ""

    assert not dir1.exists()
    assert not dir2.exists()
    assert dir3.exists()


def test_remove_parents_basic1(tmpdir_factory):
    """Passing an existing, empty directory with -p removes it and its parents."""
    data = Path(tmpdir_factory.mktemp("data"))
    dir1 = data / "dir1"
    dir2 = data / "dir2"
    dir3 = data / "dir3"

    dir1.mkdir()
    dir2.mkdir()
    dir3.mkdir()

    assert dir1.is_dir()
    assert dir2.is_dir()
    assert dir3.is_dir()

    with chdir(data):
        ret = run(["rmdir", "-p", str(dir1), str(dir2), str(dir3)])
    assert ret.stdout == ""
    # We should get two errors about removing `data` and one about removing
    # the parent directory of `data`.
    assert ret.stderr != ""

    assert not data.exists()  # Removed implicitly because it's the parent.
    assert not dir1.exists()
    assert not dir2.exists()
    assert not dir3.exists()


def test_remove_parents_complex(tmpdir_factory):
    """Passing an existing, empty directory with -p removes it and its parents."""
    data = Path(tmpdir_factory.mktemp("data"))
    dir1 = data / "dir1"
    dir2 = data / "dir2"
    dir3 = data / "dir3"
    dir3_inner = dir3 / "inner"

    dir1.mkdir()
    dir2.mkdir()
    dir3.mkdir()
    dir3_inner.mkdir()

    assert dir1.is_dir()
    assert dir2.is_dir()
    assert dir3.is_dir()
    assert dir3_inner.is_dir()

    with chdir(data):
        ret = run(["rmdir", "-p", str(dir1), str(dir3_inner), str(dir2)])
    assert ret.stdout == ""
    # We should get two errors about removing `data` and one about removing
    # the parent directory of `data`.
    assert ret.stderr != ""

    assert not data.exists()  # Removed implicitly because it's the parent.
    assert not dir1.exists()
    assert not dir2.exists()
    assert not dir3.exists()
    assert not dir3_inner.exists()


def test_nonexistent_paths(tmpdir_factory):
    """Passing nonexistent paths reports an error."""
    data = Path(tmpdir_factory.mktemp("data"))
    dir1 = data / "dir1"
    dir2 = data / "dir2"
    assert not dir1.exists()
    assert not dir2.exists()

    with chdir(data):
        ret = check_fail(["rmdir", str(dir1)])
    assert ret.stdout == ""
    assert ret.stderr != ""

    with chdir(data):
        ret = check_fail(["rmdir", str(dir1), str(dir2)])
    assert ret.stdout == ""
    assert ret.stderr != ""


def test_remove_parents_partial_fail(tmpdir_factory):
    """Passing an existing empty directory with non-empty parents, and -p
       removes as much as possible and returns an error."""
    data = Path(tmpdir_factory.mktemp("data"))
    dir1 = data / "dir1"
    dir2 = data / "dir2"
    dir3 = data / "dir3"
    dir3_inner = dir3 / "inner"
    dir3_inner_deep = dir3_inner / "deep"
    dir3_inner2 = dir3 / "inner2"

    dir1.mkdir()
    dir2.mkdir()
    dir3.mkdir()
    dir3_inner.mkdir()
    dir3_inner_deep.mkdir()
    dir3_inner2.mkdir()

    # At this point we should have these directories:
    # /.../dir1/                (empty)
    # /.../dir2/                (empty)
    # /.../dir3/                (not empty)
    # /.../dir3/inner/          (not empty)
    # /.../dir3/inner/deep/     (empty)
    # /.../dir3/inner2/         (empty)
    assert dir1.is_dir()
    assert dir2.is_dir()
    assert dir3.is_dir()
    assert dir3_inner.is_dir()
    assert dir3_inner_deep.is_dir()
    assert dir3_inner2.is_dir()

    # Given the above directory structure, we should be able to remove
    # dir1, fail to remove dir3, and then be able to remove dir2.
    with chdir(data):
        ret = run(["rmdir", "-p", str(dir1), str(dir3_inner_deep), str(dir2)])
    assert ret.stdout == ""
    # We should get one error about `data` not being empty, one about
    # `dir3` not being empty, then another about `data` not being empty.
    assert ret.stderr != ""

    # At this point, we should be left with just /.../dir3/inner2/.
    assert not dir1.exists()
    assert not dir2.exists()
    assert dir3.exists()
    assert not dir3_inner.exists()
    assert not dir3_inner_deep.exists()
    assert dir3_inner2.exists()

    assert ret.returncode != 0
