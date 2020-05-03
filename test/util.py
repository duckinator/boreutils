import subprocess

def check(args):
    return subprocess.run(args, capture_output=True, text=True, check=True)

def run(args):
    return subprocess.run(args, capture_output=True, text=True)
