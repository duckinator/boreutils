import unittest
from .util import run, fails

class BasenameTestCase(unittest.TestCase):
    # https://pubs.opengroup.org/onlinepubs/9699919799/utilities/self.basename.html
    basename = "./bin/basename"

    def test_running_correct_program(self):
        self.assertEqual(run([self.basename, "--version"]).stdout.split(' ')[0], 'Boreutils')

    def test_no_args(self):
        # No args => error of the form "self.basename: ..." or "/path/to/basename: ..."
        self.assertTrue(fails([self.basename]).stderr.split(' ')[0].endswith("basename:"))

    def test_help(self):
        # Passing -h or --help => print help text.
        self.assertEqual(fails([self.basename, "-h"]).stdout.split(' ')[0], 'Usage:')
        self.assertEqual(fails([self.basename, "--help"]).stdout.split(' ')[0], 'Usage:')

    # Test the various steps:

    def test_step1(self):
        # 1. Empty string results in an empty string.
        self.assertEqual(run([self.basename, ""]).stdout.strip(), "")

    def test_step2(self):
        # 2. We _do not_ skip steps 3-6 if given "//", so this should return "/".
        #    If we do skip step 3-6, this should return "//"!
        self.assertEqual(run([self.basename, "//"]).stdout.strip(), "/")

    def test_step3(self):
        # 3. If string is entirely slash characters, we get a single slash.
        self.assertEqual(run([self.basename, "///"]).stdout.strip(), "/")

    def test_step4(self):
        # 4. Remove trailing slash characters.
        self.assertEqual(run([self.basename, "owo/"]).stdout.strip(), "owo")
        #! Potential edge case if we change behavior for step 2.
        self.assertEqual(run([self.basename, "owo//"]).stdout.strip(), "owo")
        self.assertEqual(run([self.basename, "owo///"]).stdout.strip(), "owo")

    def test_step5(self):
        # 5. If there are remaining slash characters, remove everything up to and
        #    including the last slash.
        self.assertEqual(run([self.basename, "/a/b/c/d/owo"]).stdout.strip(), "owo")
        #! Potential edge case exercising steps 4+5 together.
        self.assertEqual(run([self.basename, "/a/b/c/d/owo///"]).stdout.strip(), "owo")
        self.assertEqual(run([self.basename, "///a/b/c/d/owo///"]).stdout.strip(), "owo")
