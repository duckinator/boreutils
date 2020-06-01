"""
Tests for `ish`.
"""

import subprocess
from helpers import run


def ish(cmd):
    p1 = subprocess.Popen(["echo", cmd], stdout=subprocess.PIPE)
    p2 = subprocess.Popen(["./bin/ish", "-q"], stdin=p1.stdout,
                          stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p1.stdout.close()
    output = p2.communicate()
    if output[0]:
        stdout = output[0].decode()
    else:
        stdout = ''

    if output[1]:
        stderr = output[1].decode()
    else:
        stderr = ''

    return {'stdout': stdout, 'stderr': stderr, 'process': p2}


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
    """Split between multiple functions below."""
    pass


def test_strings():
    """Test handling of quoted strings."""
    assert ish("setenv X 'owo wats this'\necho ${X}")['stdout'] == "owo wats this\n"
    assert ish('setenv X "owo wats this"\necho ${X}')['stdout'] == "owo wats this\n"
    assert ish("echo 'hello, ''world!'")['stdout'] == "hello, world!\n"
    assert ish('echo "hello, ""world!"')['stdout'] == "hello, world!\n"
    assert ish('echo "hello, "\'world!\'')['stdout'] == "hello, world!\n"
    assert ish('echo \'hello, \'"world!"')['stdout'] == "hello, world!\n"
    assert ish("echo 'hello, 'world!")['stdout'] == "hello, world!\n"
    assert ish('echo "hello, "world!')['stdout'] == "hello, world!\n"
    assert ish('echo "owo \'wats this"')['stdout'] == "owo 'wats this\n"
    assert ish('echo \'owo "wats this\'')['stdout'] == 'owo "wats this\n'


def test_echo():
    """Test a basic echo command works."""
    assert ish('  echo hello, world!')['stdout'] == "hello, world!\n"
    assert ish('echo hello, world!')['stdout'] == "hello, world!\n"
    assert ish('echo "hello," "world!"')['stdout'] == "hello, world!\n"


def test_if_good():
    """Test that valid if statements work."""
    assert ish("if true a b then { echo yay } else { echo boo }")['stdout'] == "yay\n"
    assert ish("if false a b then { echo yay } else { echo boo }")['stdout'] == "boo\n"


def test_if_bad():
    """Test that invalid if statements print help text."""
    assert ish("if a b c d e f g h i j")['stderr'].startswith("Usage: ")
    assert ish("if")['stderr'].startswith("Usage: ")
    assert ish("if true a b then { echo yay } else { echo boo } x")['stderr'].startswith("Usage: ")


def test_env():
    """Test that environment variables can be set and read."""
    assert ish("setenv A B\necho ${A}\n")['stdout'] == "B\n"


def test_pipes():
    """Test that pipes work properly."""
    assert ish("echo hello, world '|' | sed 's/|/!/' | sed 's/ !/!/' | tr l w\n")['stdout'] == "hewwo, worwd!\n"
