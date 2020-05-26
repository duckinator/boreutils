"""
Tests for POSIX-compatible `cal`.

https://pubs.opengroup.org/onlinepubs/9699919799/utilities/cal.html
"""

from pathlib import Path
import time
from helpers import check_version, check, run


def test_version():
    """Check that we're using Boreutil's implementation."""
    assert check_version("cal")


def test_missing_args():
    """Nothing to test: `cal` accepts 0-2 arguments."""
    pass


def test_extra_args():
    """Too many args => error of the form `cal: ...`"""
    assert run(["cal", "a", "b", "c"]).stderr.startswith("cal:")
    assert run(["cal", "a", "b", "c"]).returncode > 0


def test_help():
    """Passing -h or --help should print help text."""
    assert run(["cal", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["cal", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["cal", "-h"]).returncode > 0
    assert run(["cal", "--help"]).returncode > 0


def test_main():
    """Running `cal <month> <year>` should return the correct values and
       exit with a return code of 0."""
    assert check(["cal", "9", "1752"]).stdout == Path('test/test_cal/1752-9.txt').read_text()
    assert check(["cal", "2008"]).stdout == Path('test/test_cal/2008.txt').read_text()
    assert check(["cal", "2020"]).stdout == Path('test/test_cal/2020.txt').read_text()
    assert check(["cal", "8", "2008"]).stdout == Path('test/test_cal/2008-8.txt').read_text()
    assert check(["cal", "11", "2020"]).stdout == Path('test/test_cal/2020-11.txt').read_text()
    assert check(["cal", "12", "2020"]).stdout == Path('test/test_cal/2020-12.txt').read_text()

    assert check(["cal"]).stdout.splitlines()[0].strip() == time.strftime("%B %Y")
