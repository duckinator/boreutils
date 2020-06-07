#!/usr/bin/env python3

from glob import glob
from time import sleep
from random import randrange
from subprocess import run


def xdotool(*args):
    run(['xdotool', *args], check=True)


def key(k):
    run(['xdotool', 'key', k])
    # Sleep between 1/1000 (0.001) and 100/1000 (0.1) milliseconds
    sleep(randrange(0, 100) / 1000)


def type_key(k):
    assert len(k) == 1
    run(['xdotool', 'type', '--', k])
    # Sleep between 1/1000 (0.001) and 100/1000 (0.1) milliseconds
    sleep(randrange(0, 100) / 1000)


def type(string):
    for k in string:
        type_key(k)
    sleep(1)


type("echo ${SHELL}\n")
type("${SHELL} --version\n")
type("echo 'echo ${0} ${1} ${2} ${3}' | ish -qx a b 'c d e' f\n")

for exe in glob("bin/*"):
    exe = exe.split("/")[1]
    if exe == "echo":
        type('echo "This doesn\'t support --version, as per POSIX."\n')
    elif exe == "ish":
        continue
    else:
        type(f"{exe} --version\n")

sleep(1)

type("man ish | less")
sleep(0.5)
type("\n")
sleep(3)
for i in range(13):
    key('Down')
    sleep(0.5)
