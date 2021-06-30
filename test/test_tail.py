"""
Tests for POSIX-compatible `tail`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/tail.html
"""

from pathlib import Path
import subprocess
from helpers import check, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("tail")


def test_missing_args():
    """Nothing to test: `tail` accepts any number of arguments."""
    pass


def test_extra_args():
    """Specifying multiple files results in an error."""
    assert run(["tail", "a", "b"]).stderr.startswith("tail: Extra argument")


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["tail", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["tail", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["tail", "-h"]).returncode > 0
    assert run(["tail", "--help"]).returncode > 0


def test_main():
    """Split across test_file() and test_stdin()"""
    pass


def test_file():
    """Passing file paths should read from file."""
    path1 = Path("src/basename.c").resolve()
    lines = path1.read_text().splitlines()

    expected = "\n".join(lines[-10:]) + "\n"
    assert check(["tail", str(path1)]).stdout == expected

    expected = "\n".join(lines[-3:]) + "\n"
    assert check(["tail", "-n", "3", str(path1)]).stdout == expected

    expected = "\n".join(lines[3:]) + "\n"
    assert check(["tail", "-n", "+3", str(path1)]).stdout == expected


def test_stdin():
    """Passing no arguments should read from stdin."""
    with subprocess.Popen(["printf", "1 abc\n2 def\n3 ghi\n4\n5\n6\n7\n8\n9\n10\n11\n12"], stdout=subprocess.PIPE) as p1:
        with subprocess.Popen(["./bin/tail"], stdin=p1.stdout, stdout=subprocess.PIPE) as p2:
            p1.stdout.close()
            output = p2.communicate()[0].decode()
            assert output == "3 ghi\n4\n5\n6\n7\n8\n9\n10\n11\n12"

    with subprocess.Popen(["printf", "abcdefhij1234567890"], stdout=subprocess.PIPE) as p1:
        with subprocess.Popen(["./bin/tail", "-c", "11"], stdin=p1.stdout, stdout=subprocess.PIPE) as p2:
            p1.stdout.close()
            output = p2.communicate()[0].decode()
            assert output == "34567890"

    with subprocess.Popen(["printf", "abcdefhij1234567890"], stdout=subprocess.PIPE) as p1:
        with subprocess.Popen(["./bin/tail", "-c", "-11"], stdin=p1.stdout, stdout=subprocess.PIPE) as p2:
            p1.stdout.close()
            output = p2.communicate()[0].decode()
            assert output == "j1234567890"
