import unittest
import os
import subprocess


class TestListAllFunctions(unittest.TestCase):

    def run_pass_test(self, bitcodefile, new_entrypoint, assertions):
        self.assertIn("LLVMBIN", os.environ, "Path to llvm-bin not set")
        self.assertIn("KLEEBIN", os.environ, "Path to klee-bin not set")

        modfilename = "bin/mod-" + new_entrypoint + ".bc"

        subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "-encapsulatesymbolic", bitcodefile,
            "-encapsulatedfunction", new_entrypoint,
            "-o", modfilename])

        out = subprocess.check_output(
            [os.environ["KLEEBIN"] + "/klee", modfilename],
            stderr=subprocess.STDOUT)

        for assertion in assertions:
            self.assertIn(assertion, out.decode("utf-8"))

    def test_not42(self):
        self.run_pass_test(
            "bin/not42.bc", "not42", ["ASSERTION FAIL: i != 42"])

    def test_singlechar(self):
        self.run_pass_test(
            "bin/assertions.bc", "singlechar", ["ASSERTION FAIL: c != 'x'"])

    def test_largeint(self):
        self.run_pass_test(
            "bin/assertions.bc", "largeint",
            ["ASSERTION FAIL: n != 9223372036854775807"])

    @unittest.skip("KLEE does not support doubles")
    def test_singledouble(self):
        self.run_pass_test("bin/assertions.bc", "singledouble", [])

    def test_singleint(self):
        self.run_pass_test(
            "bin/assertions.bc", "singleint", ["ASSERTION FAIL: i != -1"])

    def test_withreturn(self):
        self.run_pass_test(
            "bin/assertions.bc", "withreturn", ["ASSERTION FAIL: i != 42"])

    def test_twoints(self):
        self.run_pass_test(
            "bin/assertions.bc", "twoints", [
                "ASSERTION FAIL: n != 1",
                "ASSERTION FAIL: i != 0",
                "ASSERTION FAIL: (n * -1) != i"
            ])

    def test_indirect(self):
        self.run_pass_test(
            "bin/assertions.bc", "indirect", ["ASSERTION FAIL: i != -1"])

    def test_singlepointer(self):
        self.run_pass_test(
            "bin/assertions.bc", "singlepointer", ["ASSERTION FAIL: *i != 42"])

    def test_singlestruct(self):
        self.run_pass_test(
            "bin/assertions.bc", "singlestruct", [
                "ASSERTION FAIL: p.i != 21",
                "ASSERTION FAIL: p.c != 'h'",
                "ASSERTION FAIL: p.i != 1 && p.c != 'e'"
            ])

    def test_singlestructpointer(self):
        self.run_pass_test(
            "bin/assertions.bc", "singlestructpointer", [
                "ASSERTION FAIL: p->i != 21",
                "ASSERTION FAIL: p->c != 'h'",
                "ASSERTION FAIL: p->i != 1 && p->c != 'e'"
            ])

    def test_simplestring(self):
        self.run_pass_test(
            "bin/assertions.bc", "simplestring",
            ["ASSERTION FAIL: strcmp(str, greeting) != 0"])

    def test_singlebool(self):
        self.run_pass_test(
            "bin/assertions.bc", "singlebool", ["ASSERTION FAIL: a"])

    def test_compareints(self):
        self.run_pass_test(
            "bin/assertions.bc", "compareints", ["ASSERTION FAIL: a <= b"])
