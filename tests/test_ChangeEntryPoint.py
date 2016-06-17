import unittest
import os
import subprocess


class TestListAllFunctions(unittest.TestCase):

    def run_pass_test(self, bitcodefile, new_entrypoint, expected):
        self.assertTrue("LLVMBIN" in os.environ, "Path to llvm-bin not set")

        modfilename = "bin/mod-" + new_entrypoint + ".bc"

        subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "-changeentrypoint", bitcodefile,
            "-newentryfunction", new_entrypoint,
            "-o", modfilename])

        out = subprocess.check_output(
            [os.environ["LLVMBIN"] + "/lli", modfilename])

        self.assertEqual(out.decode("utf-8"), expected + "\n")

    def test_german(self):
        self.run_pass_test("bin/greetings.bc", "german", "Hallo Welt!")

    def test_spanish(self):
        self.run_pass_test("bin/greetings.bc", "spanish", "Hola mundo")

    def test_french(self):
        self.run_pass_test("bin/greetings.bc", "french", "Bonjour le monde!")
