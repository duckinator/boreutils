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

    # == Potential problem years:
    # === Month POSIX cal transitions from Julian -> Gregorian calendar.
    assert check(["cal", "9", "1752"]).stdout == Path('test/test_cal/1752-9.txt').read_text()
    # === Y2K
    assert check(["cal", "2000"]).stdout == Path("test/test_cal/2000.txt").read_text()
    # === 2038
    assert check(["cal", "2038"]).stdout == Path("test/test_cal/2038.txt").read_text()
    # === 2100 is not a leap year, but some leap year algorithms think it is.
    assert check(["cal", "2100"]).stdout == Path("test/test_cal/2100.txt").read_text()

    # One year per century for 1800-2900.
    # The years chosen are prime numbers, to ensure they can not be leap years.
    # The specific year numbers for each century were chosen arbitrarily.
    assert check(["cal", "1801"]).stdout == Path("test/test_cal/1801.txt").read_text()
    assert check(["cal", "1907"]).stdout == Path("test/test_cal/1907.txt").read_text()
    assert check(["cal", "2003"]).stdout == Path("test/test_cal/2003.txt").read_text()
    assert check(["cal", "2111"]).stdout == Path("test/test_cal/2111.txt").read_text()
    assert check(["cal", "2207"]).stdout == Path("test/test_cal/2207.txt").read_text()
    assert check(["cal", "2309"]).stdout == Path("test/test_cal/2309.txt").read_text()
    assert check(["cal", "2459"]).stdout == Path("test/test_cal/2459.txt").read_text()
    assert check(["cal", "2521"]).stdout == Path("test/test_cal/2521.txt").read_text()
    assert check(["cal", "2609"]).stdout == Path("test/test_cal/2609.txt").read_text()
    assert check(["cal", "2711"]).stdout == Path("test/test_cal/2711.txt").read_text()
    assert check(["cal", "2819"]).stdout == Path("test/test_cal/2819.txt").read_text()
    assert check(["cal", "2917"]).stdout == Path("test/test_cal/2917.txt").read_text()

    # One year per eon for 3000 to 9000.
    # The years chosen are prime numbers, to ensure they can not be leap years.
    # The specific year numbers for each eon were chosen arbitrarily.
    assert check(["cal", "3001"]).stdout == Path("test/test_cal/3001.txt").read_text()
    assert check(["cal", "4003"]).stdout == Path("test/test_cal/4003.txt").read_text()
    assert check(["cal", "5009"]).stdout == Path("test/test_cal/5009.txt").read_text()
    assert check(["cal", "6011"]).stdout == Path("test/test_cal/6011.txt").read_text()
    assert check(["cal", "7019"]).stdout == Path("test/test_cal/7019.txt").read_text()
    assert check(["cal", "8039"]).stdout == Path("test/test_cal/8039.txt").read_text()
    assert check(["cal", "9041"]).stdout == Path("test/test_cal/9041.txt").read_text()

    # Year 9999 support is the last year required for POSIX compliance.
    assert check(["cal", "9999"]).stdout == Path('test/test_cal/9999.txt').read_text()

    # Check that individual months work.
    assert check(["cal", "1", "2008"]).stdout == Path('test/test_cal/2008-1.txt').read_text()
    assert check(["cal", "2", "2020"]).stdout == Path('test/test_cal/2020-2.txt').read_text()
    assert check(["cal", "12", "2100"]).stdout == Path('test/test_cal/2100-12.txt').read_text()

    # No arguments should result in the current month.
    # We don't check the contents of the current month, just the name.
    assert check(["cal"]).stdout.splitlines()[0].strip() == time.strftime("%B %Y")
