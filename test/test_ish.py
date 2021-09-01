"""
Tests for `ish`.
"""

import subprocess
from pathlib import Path
from helpers import run


def ish(cmd, args=None):
    if args is None:
        args = []

    with subprocess.Popen(["echo", cmd], stdout=subprocess.PIPE) as p1:
        with subprocess.Popen(["./bin/ish", "-q", *args], stdin=p1.stdout,
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE) as p2:
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

            return {'stdout': stdout,
                    'stderr': stderr,
                    'process': p2,
                    'returncode': p2.returncode}


def ishx(cmd, args=None):
    if args is None:
        args = []
    return ish(cmd, ['-x', *args])


def test_version():
    """Check that -v works."""
    assert run(["ish", "-v"]).stdout.startswith("ish v")
    assert ish("echo ${SHELL}")['stdout'].endswith("/ish\n")


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


def test_cd():
    """Test `cd` behavior."""
    cwd = Path.cwd()
    assert ishx("pwd\ncd ..\npwd\ncd -\npwd\n")['stdout'] == '\n'.join([
        str(cwd),
        str(cwd.parent),
        str(cwd),
        '',
    ])


def test_exit():
    """Test `exit` behavior."""
    assert ishx("exit")['returncode'] == 0
    assert ishx("exit 1")['returncode'] == 1
    assert ishx("exit 123")['returncode'] == 123


def test_returncode():
    """Test return codes are set properly."""
    assert ishx("true")['returncode'] == 0
    assert ishx("false")['returncode'] == 1


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

    command = "setenv X /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/root/bin\n" +\
              "echo ${X} | tr ':' '\\n' | awk '{print length, $0}' | sort -n | cut -f2- -d' '\n"
    assert ish(command)['stdout'] == "/usr/bin\n/root/bin\n/usr/sbin\n/usr/local/bin\n/usr/local/sbin\n"


def test_preset_variables():
    """Test that $SHELL, $0-$<argc - 1>, etc are all set."""
    assert ishx("echo ${SHELL}")['stdout'] == "./bin/ish\n"
    assert ishx("echo ${0}")['stdout'] == "./bin/ish\n"

    args = ['1', '2 owo', '3', '4', '5', '6', '7', '8', '9', '10']
    assert ishx("echo ${0}", args=args)['stdout'] == "./bin/ish\n"
    for i in range(0, 10):
        assert ishx("echo ${" + str(i + 1) + "}", args=args)['stdout'] == args[i] + "\n"

    assert ishx(
        "echo ${0} ${1} ${2} ${3} ${4} ${5} ${6} ${7} ${8} ${9} ${10}",
        args=args,
    )['stdout'] == "./bin/ish " + " ".join(args) + "\n"
