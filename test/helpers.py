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


def check(cmd):
    with _modified_path():
        return subprocess.run(cmd, capture_output=True, text=True, check=True)

def check_version(tool):
    return check([tool, "--version"]).stdout.split(' ')[1] == '(Boreutils)'

def run(cmd):
    with _modified_path():
        return subprocess.run(cmd, capture_output=True, text=True)
