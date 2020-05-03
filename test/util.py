import subprocess

class TestResult:
    def __init__(self, result):
        self.raw_result = result
        self.stdout = result.stdout.decode()
        self.stderr = result.stderr.decode()

class TestFailure(Exception):
    pass

def assert_eq(actual, expected):
    if expected != actual:
        # FIXME: Make this less of a disaster.
        message = f"\n\nExpected:\n    {expected!r}\nGot:\n    {actual!r}"
        raise TestFailure(message)

def assert_true(actual):
    assert_eq(actual, True)

def run(args, should_fail=False):
    result = subprocess.run(args, capture_output=True)
    if should_fail:
        assert result.returncode > 0
    else:
        assert result.returncode == 0
    return TestResult(result)

def fails(args):
    return run(args, should_fail=True)

def prints_help(args):
    stderr = run(args, should_fail=True).stderr.decode()
    print(stderr)
    return stderr.startswith("Usage: ")
