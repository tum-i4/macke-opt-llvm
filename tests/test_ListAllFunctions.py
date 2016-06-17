import unittest
import json
import os
import subprocess


class TestListAllFunctions(unittest.TestCase):

    def run_pass_test(self, bitcodefile, expected):
        self.assertTrue("LLVMBIN" in os.environ, "Path to llvm-bin not set")

        jsonout = subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "-listallfunctions", bitcodefile,
            "-disable-output"])

        self.assertEqual(json.loads(jsonout.decode("utf-8")), expected)

    def test_divisible(self):
        expected = [
            'divisible_by_10', 'divisible_by_2', 'divisible_by_3',
            'divisible_by_30', 'divisible_by_4', 'divisible_by_5',
            'divisible_by_6', 'main']

        self.run_pass_test("bin/divisible.bc", expected)

    def test_greetings(self):

        expected = ['english', 'french', 'german', 'main', 'spanish']

        self.run_pass_test("bin/greetings.bc", expected)
