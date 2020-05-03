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


def check(args):
    with _modified_path():
        return subprocess.run(args, capture_output=True, text=True, check=True)

def run(args):
    with _modified_path():
        return subprocess.run(args, capture_output=True, text=True)
