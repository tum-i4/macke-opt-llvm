import unittest
import os
import re
import subprocess


class TestIntegration(unittest.TestCase):

    skipworking = False
    ptrforks = 10

    def batch_run(
            self, bitcodefile, analyzedfunction, errcount, duplicatedErrors):
        self.assertIn("LLVMBIN", os.environ, "Path to llvm-bin not set")
        self.assertIn("KLEEBIN", os.environ, "Path to klee-bin not set")

        # Determine the filenames
        symencfile = "bin/int-sym-%s.bc" % analyzedfunction
        prependedfile = "bin/int-prep-%s.bc" % analyzedfunction

        # First encapsulate the analyzed function symbolically
        subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "-encapsulatesymbolic", bitcodefile,
            "-encapsulatedfunction", analyzedfunction,
            "-o", symencfile])

        # then pass the new file to KLEE
        firstklee = subprocess.check_output([
            os.environ["KLEEBIN"] + "/klee",
            "--optimize", "--emit-all-errors",
            "--only-output-states-covering-new",
            symencfile],
            stderr=subprocess.STDOUT)

        # If compiler decides to use pointer values, increment total count
        if errcount * self.ptrforks == firstklee.count(b"KLEE: ERROR: "):
            errcount *= self.ptrforks

        # Check the number of errors in the first klee round
        self.assertEqual(errcount, firstklee.count(b"KLEE: ERROR: "))

        # Find the directory with all error test cases
        firstkleedir = re.search(
            r"^KLEE: output directory is \"(.*)\"",
            firstklee.decode("utf-8")).group(1)

        # Then prepend all errors to the function
        subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "-preprenderror", symencfile,
            "-prependtofunction", analyzedfunction,
            "-previouskleerundirectory", firstkleedir,
            # "-disable-verify",
            "-o", prependedfile])

        # And finally run KLEE again with prepended errors
        secondklee = subprocess.check_output([
            os.environ["KLEEBIN"] + "/klee",
            "--optimize", "--emit-all-errors",
            "--only-output-states-covering-new",
            prependedfile],
            stderr=subprocess.STDOUT)

        # print(secondklee.decode("utf-8"))

        # Find the directory with the newly generated testcases
        secondkleedir = re.search(
            r"^KLEE: output directory is \"(.*)\"",
            secondklee.decode("utf-8")).group(1)

        # Count the generated testcases
        testcount = int(re.search(
            r"KLEE: done: generated tests = (\d+)",
            secondklee.decode("utf-8")).group(1))

        # Check that the number of errors was exactly doubled
        self.assertEqual(errcount * 2, secondklee.count(b"KLEE: ERROR: "))

        # And run the ktest tool on all of them to check the ouput
        ktests = subprocess.check_output(
            [os.environ["KLEEBIN"] + "/ktest-tool"] +
            [secondkleedir + "/test%06d.ktest" % i
                for i in range(1, testcount + 1)])

        # print(ktests.decode("utf-8"))

        for err in duplicatedErrors:
            if isinstance(err, bytes):
                self.assertEqual(2, ktests.count(err),
                                 "%s is missing in %s after %s " %
                                 (err, secondkleedir, firstkleedir))
            else:
                self.assertEqual(2, sum([ktests.count(e) for e in err]),
                                 "%s is missing in %s after %s " %
                                 (err, secondkleedir, firstkleedir))

    @unittest.skipIf(skipworking, "works")
    def test_int_not42(self):
        self.batch_run(
            "bin/not42.bc", "not42", 1, [b"'*\\x00\\x00\\x00'"])

    @unittest.skipIf(skipworking, "works")
    def test_int_singlechar(self):
        self.batch_run(
            "bin/assertions.bc", "singlechar", 1, [b"'x'"])

    @unittest.skipIf(skipworking, "works")
    def test_int_largeint(self):
        self.batch_run(
            "bin/assertions.bc", "largeint", 1,
            [b"'\\xff\\xff\\xff\\xff\\xff\\xff\\xff\\x7f'"])

    @unittest.skipIf(skipworking, "works")
    def test_int_singleint(self):
        self.batch_run(
            "bin/assertions.bc", "singleint", 1, [b"'\\xff\\xff\\xff\\xff'"])

    @unittest.skipIf(skipworking, "works")
    def test_int_withreturn(self):
        self.batch_run(
            "bin/assertions.bc", "withreturn", 1, [b"'*\\x00\\x00\\x00'"])

    @unittest.skipIf(skipworking, "works")
    def test_int_twoints(self):
        self.batch_run(
            "bin/assertions.bc", "twoints", 3,
            [b"'\\x0c\\x00\\x00\\x00'", b"'\\r\\x00\\x00\\x00'",
             b"'\\xff\\xff\\xff\\xff'"])

    @unittest.skipIf(skipworking, "works")
    def test_int_indirect(self):
        self.batch_run(
            "bin/assertions.bc", "indirect", 1, [b"'\\xff\\xff\\xff\\xff'"])

    @unittest.skipIf(skipworking, "works")
    def test_int_singlepointer(self):
        self.batch_run(
            "bin/assertions.bc", "singlepointer", 1,
            [b"'*\\x00\\x00\\x00'"])

    @unittest.skipIf(skipworking, "works")
    def test_int_singlestruct(self):
        self.batch_run(
            "bin/assertions.bc", "singlestruct", 3, [
                [b"'\\x01\\x00\\x00\\x00e\\xff\\xff\\xff'",
                 b"'\\x01\\x00\\x00\\x00e\\x00\\x00\\x00'"],
                [b"'\\x15\\x00\\x00\\x00\\xff\\xff\\xff\\xff'",
                 b"'\\x15\\x00\\x00\\x00\\x00\\x00\\x00\\x00'"],
                b"'\\x00\\x00\\x00\\x00h\\x00\\x00\\x00'"
            ])

    @unittest.skipIf(skipworking, "works")
    def test_int_singlestructpointer(self):
        self.batch_run(
            "bin/assertions.bc", "singlestructpointer", 3, [
                [b"'\\x01\\x00\\x00\\x00e\\xff\\xff\\xff'",
                 b"'\\x01\\x00\\x00\\x00e\\x00\\x00\\x00'"],
                [b"'\\x15\\x00\\x00\\x00\\xff\\xff\\xff\\xff'",
                 b"'\\x15\\x00\\x00\\x00\\x00\\x00\\x00\\x00'"],
                b"'\\x00\\x00\\x00\\x00h\\x00\\x00\\x00'"
            ])

    @unittest.skipIf(skipworking, "works")
    def test_int_simplestring(self):
        self.batch_run(
            "bin/assertions.bc", "simplestring", 1,
            [b"'Hi\\x00'"])

    @unittest.skipIf(skipworking, "works")
    def test_int_singlebool(self):
        self.batch_run("bin/assertions.bc", "singlebool", 1, [b"'\\x00'"])
