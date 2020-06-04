"""
Tests for POSIX-compatible `head`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/head.html
"""

from pathlib import Path
import subprocess
from helpers import check, check_version, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("head")


def test_missing_args():
    """Nothing to test: `head` accepts any number of arguments."""
    pass


def test_extra_args():
    """Nothing to test: `head` accepts any number of arguments."""
    pass


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["head", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["head", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["head", "-h"]).returncode > 0
    assert run(["head", "--help"]).returncode > 0


def test_main():
    """Split across test_file() and test_stdin()"""
    pass


def test_file():
    """Passing file paths should read from file."""
    path1 = Path("src/basename.c").resolve()
    path2 = Path("src/cal.c").resolve()
    path3 = Path("src/head.c").resolve()
    expected_path1 = "\n".join(path1.read_text().splitlines()[0:10]) + "\n"
    expected_path2 = "\n".join(path2.read_text().splitlines()[0:10]) + "\n"
    expected_path3 = "\n".join(path3.read_text().splitlines()[0:10]) + "\n"
    expected_all = expected_path1 + expected_path2 + expected_path3
    assert check(["head", str(path1)]).stdout == expected_path1
    assert check(["head", str(path1), str(path2), str(path3)]).stdout == expected_all


def test_file_n11():
    """Passing `-n 11` and file paths should read 11 lines from the files."""
    path1 = Path("src/basename.c").resolve()
    path2 = Path("src/cal.c").resolve()
    path3 = Path("src/head.c").resolve()
    expected_path1 = "\n".join(path1.read_text().splitlines()[0:11]) + "\n"
    expected_path2 = "\n".join(path2.read_text().splitlines()[0:11]) + "\n"
    expected_path3 = "\n".join(path3.read_text().splitlines()[0:11]) + "\n"
    expected_all = expected_path1 + expected_path2 + expected_path3
    assert check(["head", "-n", "11", str(path1)]).stdout == expected_path1
    assert check(["head", "-n", "11", str(path1), str(path2), str(path3)]).stdout == expected_all


def test_stdin():
    """Passing no arguments should read from stdin."""
    p1 = subprocess.Popen(["printf", "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12"], stdout=subprocess.PIPE)
    p2 = subprocess.Popen(["./bin/head"], stdin=p1.stdout, stdout=subprocess.PIPE)
    p1.stdout.close()
    output = p2.communicate()[0].decode()
    assert output == "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n"


def test_stdin_n11():
    """Passing only `-n 11` should read 11 lines from stdin."""
    p1 = subprocess.Popen(["printf", "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12"], stdout=subprocess.PIPE)
    p2 = subprocess.Popen(["./bin/head", "-n", "11"], stdin=p1.stdout, stdout=subprocess.PIPE)
    p1.stdout.close()
    output = p2.communicate()[0].decode()
    assert output == "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n"
