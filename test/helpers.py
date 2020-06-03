"""
Helper functions for the Boreutils test suite.
"""

from contextlib import contextmanager
from pathlib import Path
import os
import subprocess


@contextmanager
def _modified_path():
    current_dir = Path(__file__).resolve().parent
    bin_dir = (current_dir / '..' / 'bin').resolve()
    original_path = os.environ['PATH']

    try:
        os.environ['PATH'] = f'{bin_dir}:{original_path}'
        yield
    finally:
        os.environ['PATH'] = original_path


def check(cmd, **kwargs):
    """Run a command, capture the output as text, check it had a 0 returncode,
       and return the +CompletedProcess+ object."""
    with _modified_path():
        return subprocess.run(cmd, capture_output=True, text=True, check=True, **kwargs)


def check_version(tool):
    """Check if running `{tool} --version` has '(Boreutils)' as the second word."""
    return check([tool, "--version"]).stdout.split(' ')[1] == '(Boreutils)'


def run(cmd, **kwargs):
    """Run a command, capture the output as text, _don't_ check the return code,
       and return the +CompletedProcess+ object."""
    with _modified_path():
        return subprocess.run(cmd, capture_output=True, text=True, check=False, **kwargs)


def check_fail(cmd, **kwargs):
    """Same as run(cmd, **kwargs) but asserts the returncode is >0."""
    ret = run(cmd, **kwargs)
    assert ret.returncode > 0
    return ret
