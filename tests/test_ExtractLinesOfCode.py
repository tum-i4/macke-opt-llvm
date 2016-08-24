import unittest
import json
import os
import subprocess


class TestListAllFunctions(unittest.TestCase):

    def run_pass_test(self, bitcodefile):
        self.assertIn("LLVMBIN", os.environ, "Path to llvm-bin not set")

        jsonout = subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "-extractlinesofcode", bitcodefile,
            "-disable-output"])

        return json.loads(jsonout.decode("utf-8"))

    def test_divisible(self):
        output = self.run_pass_test("bin/divisible.bc")

        self.assertEqual(
            set(output.keys()),
            {'divisible_by_10', 'divisible_by_2', 'divisible_by_3',
             'divisible_by_30', 'divisible_by_4', 'divisible_by_5',
             'divisible_by_6', 'main'})

        ll = set(tuple(line)
                 for _, file in output.items() for _, line in file.items())
        self.assertEqual(ll, {(7, 8), (27, 28, 29), (19, 20, 21), (15, 16),
                              (11, 12, 13), (31, 32, 34, 35, 36, 38),
                              (3, 4), (23, 24, 25)})

    def test_greetings(self):
        output = self.run_pass_test("bin/greetings.bc")

        self.assertEqual(
            set(output.keys()),
            {'english', 'french', 'german', 'main', 'spanish'})

        ll = set(tuple(line)
                 for _, file in output.items() for _, line in file.items())
        self.assertEqual(ll, {(9, 10), (24,), (14, 15), (4, 5), (19, 20)})
