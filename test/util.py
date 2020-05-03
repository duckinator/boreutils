import subprocess

class TestResult:
    def __init__(self, result):
        self.raw_result = result
        self.returncode = result.returncode
        self.stdout = result.stdout.decode()
        self.stderr = result.stderr.decode()

def run(args, should_fail=False):
    result = subprocess.run(args, capture_output=True)
    assert result.returncode == 0
    return TestResult(result)

def fails(args):
    result = subprocess.run(args, capture_output=True)
    assert result.returncode > 0
    return TestResult(result)
