"""
Tests for POSIX-compatible `cat`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/cat.html
"""

from pathlib import Path
import subprocess
from helpers import check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("cat")


def test_missing_args():
    """Nothing to test: `cat` accepts any number of arguments."""
    pass


def test_extra_args():
    """Nothing to test: `cat` accepts any number of arguments."""
    pass


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["cat", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["cat", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["cat", "-h"]).returncode > 0
    assert run(["cat", "--help"]).returncode > 0


def test_main():
    """Split across test_file() and test_stdin()"""
    pass


def test_file():
    """Passing arguments should read from files."""
    path1 = Path("src/basename.c").resolve()
    path2 = Path("src/cal.c").resolve()
    path3 = Path("src/cat.c").resolve()
    full_results = path1.read_text() + path2.read_text() + path3.read_text()
    assert run(["cat", str(path1)]).stdout == path1.read_text()
    assert run(["cat", str(path1), str(path2), str(path3)]).stdout == full_results


def test_stdin():
    """Passing no arguments should read from stdin."""
    with subprocess.Popen(["echo", "owo"], stdout=subprocess.PIPE) as p1:
        with subprocess.Popen(["./bin/cat"], stdin=p1.stdout, stdout=subprocess.PIPE) as p2:
            p1.stdout.close()
            output = p2.communicate()[0].decode()
            assert output == "owo\n"
