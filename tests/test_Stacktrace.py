import unittest
import os
import re
import subprocess


class TestStacktrace(unittest.TestCase):

    def run_pass_test(self, bitcodefile, prependtofunction):
        self.assertIn("LLVMBIN", os.environ, "Path to llvm-bin not set")
        self.assertIn("KLEEBIN", os.environ, "Path to klee-bin not set")

        modfilename = "bin/mod-" + prependtofunction + ".bc"

        subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "-preprenderror", bitcodefile,
            "-prependtofunction", prependtofunction,
            "-o", modfilename])

        out = subprocess.check_output(
            [os.environ["KLEEBIN"] + "/klee", modfilename],
            stderr=subprocess.STDOUT)

        trace = list(reversed(
            re.findall("#\d{9} in (\w+) \(", out.decode("utf-8"))))

        return trace

    def test_foo(self):
        trace = self.run_pass_test("bin/klee_stacktrace.bc", "foo")
        self.assertEqual(["main", "__macke_error_foo", "foo", "bar"], trace)

    def test_bar(self):
        trace = self.run_pass_test("bin/klee_stacktrace.bc", "bar")
        self.assertEqual(["main", "foo", "__macke_error_bar", "bar"], trace)
