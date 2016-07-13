import unittest
import os
import re
import subprocess


class TestPrependError(unittest.TestCase):

    def test_symmain(self):
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
            # "-disable-verify",
            "-o", modedbitcodefile])

        out = subprocess.check_output([
            os.environ["KLEEBIN"] + "/klee",
            "--optimize", "--only-output-states-covering-new",
            modedbitcodefile],
            stderr=subprocess.STDOUT)

        print(out)
