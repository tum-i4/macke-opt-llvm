import unittest
import json
import os
import subprocess


class TestListAllFuncsTopologic(unittest.TestCase):

    def run_pass_test(self, bitcodefile, expected):
        self.assertIn("LLVMBIN", os.environ, "Path to llvm-bin not set")

        jsonout = subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "--listallfuncstopologic", bitcodefile,
            "-disable-output"])

        self.assertEqual(json.loads(jsonout.decode("utf-8")), expected)

    def test_divisible(self):
        expected = [
            'divisible_by_2', 'divisible_by_3', 'divisible_by_4',
            'divisible_by_5', 'divisible_by_6', 'divisible_by_10',
            'divisible_by_30', 'main'
        ]

        self.run_pass_test("bin/divisible.bc", expected)

    def test_doomcircle(self):

        expected = [['a', 'b', 'c'], 'main']

        self.run_pass_test("bin/doomcircle.bc", expected)
