import subprocess

def run(args, should_fail=False):
    result = subprocess.run(args, capture_output=True)
    if should_fail:
        assert result.returncode > 0
    else:
        assert result.returncode == 0
    return result

def fails(args):
    return run(args, should_fail=True)

def prints_help(args):
    stderr = run(args, should_fail=True).stderr.decode()
    print(stderr)
    return stderr.startswith("Usage: ")
