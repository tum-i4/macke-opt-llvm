import unittest
import json
import os
import subprocess


class TestExtractCallgraph(unittest.TestCase):

    def run_pass_test(self, bitcodefile, expected):
        self.assertIn("LLVMBIN", os.environ, "Path to llvm-bin not set")

        jsonout = subprocess.check_output([
            os.environ["LLVMBIN"] + "/opt",
            "-load", "bin/libMackeOpt.so",
            "-extractcallgraph", bitcodefile,
            "-disable-output"])

        self.assertEqual(json.loads(jsonout.decode("utf-8")), expected)

    def test_divisible(self):
        expected = {
            'main': {
                'calls': ['atoi', 'divisible_by_6',
                          'divisible_by_10', 'divisible_by_30'],
                '#uses': 1,
                'hasdoubleptrattr': True
            },
            'null function': {
                'calls': [
                    'divisible_by_2', 'llvm.dbg.declare', 'divisible_by_3',
                    'divisible_by_4', 'divisible_by_5', 'divisible_by_6',
                    'divisible_by_10', 'divisible_by_30', 'main', 'atoi'],
                '#uses': 0,
                'hasdoubleptrattr': False
            },
            'divisible_by_2': {
                'calls': [],
                '#uses': 5,
                'hasdoubleptrattr': False
            },
            'divisible_by_3': {
                'calls': [],
                '#uses': 3,
                'hasdoubleptrattr': False
            },
            'divisible_by_4': {
                'calls': ['divisible_by_2', 'divisible_by_2'],
                '#uses': 1,
                'hasdoubleptrattr': False
            },
            'divisible_by_5': {
                'calls': [],
                '#uses': 2,
                'hasdoubleptrattr': False
            },
            'divisible_by_6': {
                'calls': ['divisible_by_2', 'divisible_by_3'],
                '#uses': 2,
                'hasdoubleptrattr': False
            },
            'divisible_by_10': {
                'calls': ['divisible_by_2', 'divisible_by_5'],
                '#uses': 3,
                'hasdoubleptrattr': False
            },
            'divisible_by_30': {
                'calls': ['divisible_by_3', 'divisible_by_10'],
                '#uses': 2,
                'hasdoubleptrattr': False
            },
            'atoi': {
                'calls': ['external node'],
                '#uses': 2,
                'hasdoubleptrattr': False
            },
            'llvm.dbg.declare': {
                'calls': [],
                '#uses': 1,
                'hasdoubleptrattr': False
            }
        }

        self.run_pass_test("bin/divisible.bc", expected)

    def test_greetings(self):
        expected = {
            'null function': {
                'calls': ['english', 'puts', 'german',
                          'french', 'spanish', 'main'],
                '#uses': 0, 'hasdoubleptrattr': False},
            'main': {
                'calls': ['english'], '#uses': 1, 'hasdoubleptrattr': False},
            'english': {
                'calls': ['puts'], '#uses': 2, 'hasdoubleptrattr': False},
            'french': {
                'calls': ['puts'], '#uses': 1, 'hasdoubleptrattr': False},
            'german': {
                'calls': ['puts'], '#uses': 1, 'hasdoubleptrattr': False},
            'spanish': {
                'calls': ['puts'], '#uses': 1, 'hasdoubleptrattr': False},
            'puts': {
                'calls': ['external node'],
                '#uses': 5, 'hasdoubleptrattr': False}
        }

        self.run_pass_test("bin/greetings.bc", expected)
