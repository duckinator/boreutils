"""
Tests for `ish`.
"""

from helpers import check, run


def test_version():
    """Check that -v works."""
    assert run(["ish", "-v"]).stdout.startswith("ish v")


def test_missing_args():
    """Nothing to test: `ish` accepts any number of arguments."""
    pass


def test_extra_args():
    """Nothing to test: `ish` accepts any number of arguments."""
    pass


def test_help():
    """Passing -h or --help => print help text."""
    assert run(["ish", "-h"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["ish", "--help"]).stdout.split(' ')[0] == 'Usage:'
    assert run(["ish", "-h"]).returncode > 0
    assert run(["ish", "--help"]).returncode > 0


def test_main():
    """???"""
    pass
