import unittest
import os
import re
import subprocess


class TestPrependError(unittest.TestCase):

    def test_symmain_directory(self):
        self.assertIn("LLVMBIN", os.environ, "Path to llvm-bin not set")
        self.assertIn("KLEEBIN", os.environ, "Path to klee-bin not set")

        bitcodefile = "bin/klee_symmain.bc"
        prependtofunction = "faulty"
        modedbitcodefile = "bin/mod-klee_symmain.bc"

        # First run KLEE normaly
        out = subprocess.check_output([
            os.environ["KLEEBIN"] + "/klee",
            "--optimize", "--only-output-states-covering-new",
            bitcodefile],
            stderr=subprocess.STDOUT)

        # Read the directory with all error asserts
        kleedir = re.search(
            r"^KLEE: output directory is \"(.*)\"",
            out.decode("utf-8")).group(1)

        # Prepend the error summaries
        subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "-preprenderror", bitcodefile,
            "-prependtofunction", prependtofunction,
            "-previouskleerundirectory", kleedir,
            "-o", modedbitcodefile])

        out = subprocess.check_output([
            os.environ["KLEEBIN"] + "/klee",
            "--optimize", "--only-output-states-covering-new",
            modedbitcodefile],
            stderr=subprocess.STDOUT)

        self.assertTrue(b"KLEE: done: generated tests = 7" in out)
        self.assertEqual(6, out.count(b"KLEE: ERROR:"))

        out = subprocess.check_output(
            [os.environ["KLEEBIN"] + "/ktest-tool"] +
            ["bin/klee-last/test00000%d.ktest" % i for i in range(1, 8)])

        self.assertEqual(2, out.count(b"\\x15\\x00\\x00\\x00"))
        self.assertEqual(2, out.count(b"*\\x00\\x00\\x00"))
        self.assertEqual(2, out.count(b"9\\x05\\x00\\x00"))

    def test_symmain_direct_files(self):
        self.assertIn("LLVMBIN", os.environ, "Path to llvm-bin not set")
        self.assertIn("KLEEBIN", os.environ, "Path to klee-bin not set")

        bitcodefile = "bin/klee_symmain.bc"
        prependtofunction = "faulty"
        modedbitcodefile = "bin/mod-klee_symmain.bc"

        # First run KLEE normaly
        out = subprocess.check_output([
            os.environ["KLEEBIN"] + "/klee",
            "--optimize", "--only-output-states-covering-new",
            bitcodefile],
            stderr=subprocess.STDOUT)

        # Read the directory with all error asserts
        kleedir = re.search(
            r"^KLEE: output directory is \"(.*)\"",
            out.decode("utf-8")).group(1)

        # Build a list, where all .err files are named explicitly
        errfilelist = []
        for file in os.listdir(kleedir):
            if file.endswith(".err"):
                errfilelist.append("-errorfiletoprepend")
                errfilelist.append(os.path.join(kleedir, file))

        # Prepend the error summaries
        subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "-preprenderror", bitcodefile,
            "-prependtofunction", prependtofunction] +
            errfilelist +
            ["-o", modedbitcodefile])

        out = subprocess.check_output([
            os.environ["KLEEBIN"] + "/klee",
            "--optimize", "--only-output-states-covering-new",
            modedbitcodefile],
            stderr=subprocess.STDOUT)

        self.assertTrue(b"KLEE: done: generated tests = 7" in out)
        self.assertEqual(6, out.count(b"KLEE: ERROR:"))

        out = subprocess.check_output(
            [os.environ["KLEEBIN"] + "/ktest-tool"] +
            ["bin/klee-last/test00000%d.ktest" % i for i in range(1, 8)])

        self.assertEqual(2, out.count(b"\\x15\\x00\\x00\\x00"))
        self.assertEqual(2, out.count(b"*\\x00\\x00\\x00"))
        self.assertEqual(2, out.count(b"9\\x05\\x00\\x00"))
