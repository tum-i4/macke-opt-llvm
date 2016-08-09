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
                'calls': ['atoi', 'divisible_by_10', 'divisible_by_30',
                          'divisible_by_6'],
                'calledby': [],
                '#uses': 1,
                'hassingleptrarg': False,
                'hasdoubleptrarg': True,
                'isexternal': False
            },
            'null function': {
                'calls': [
                    'atoi', 'divisible_by_10', 'divisible_by_2',
                    'divisible_by_3', 'divisible_by_30', 'divisible_by_4',
                    'divisible_by_5', 'divisible_by_6', 'llvm.dbg.declare',
                    'main'],
                'calledby': [],
                '#uses': 0,
                'hassingleptrarg': False,
                'hasdoubleptrarg': False,
                'isexternal': True
            },
            'divisible_by_2': {
                'calls': [], 'calledby': [
                    'divisible_by_10', 'divisible_by_4', 'divisible_by_6'],
                '#uses': 5,
                'hassingleptrarg': False,
                'hasdoubleptrarg': False,
                'isexternal': False
            },
            'divisible_by_3': {
                'calls': [],
                'calledby': ['divisible_by_30', 'divisible_by_6'],
                '#uses': 3,
                'hassingleptrarg': False,
                'hasdoubleptrarg': False,
                'isexternal': False
            },
            'divisible_by_4': {
                'calls': ['divisible_by_2'],
                'calledby': [],
                '#uses': 1,
                'hassingleptrarg': False,
                'hasdoubleptrarg': False,
                'isexternal': False
            },
            'divisible_by_5': {
                'calls': [],
                'calledby': ['divisible_by_10'],
                '#uses': 2,
                'hassingleptrarg': False,
                'hasdoubleptrarg': False,
                'isexternal': False
            },
            'divisible_by_6': {
                'calls': ['divisible_by_2', 'divisible_by_3'],
                'calledby': ['main'],
                '#uses': 2,
                'hassingleptrarg': False,
                'hasdoubleptrarg': False,
                'isexternal': False
            },
            'divisible_by_10': {
                'calls': ['divisible_by_2', 'divisible_by_5'],
                'calledby': ['divisible_by_30', 'main'],
                '#uses': 3,
                'hassingleptrarg': False,
                'hasdoubleptrarg': False,
                'isexternal': False
            },
            'divisible_by_30': {
                'calls': ['divisible_by_10', 'divisible_by_3'],
                'calledby': ['main'],
                '#uses': 2,
                'hassingleptrarg': False,
                'hasdoubleptrarg': False,
                'isexternal': False
            },
            'atoi': {
                'calls': [],
                'calledby': ['main'],
                '#uses': 2,
                'hassingleptrarg': True,
                'hasdoubleptrarg': False,
                'isexternal': True
            },
            'llvm.dbg.declare': {
                'calls': [],
                'calledby': [],
                '#uses': 1,
                'hassingleptrarg': False,
                'hasdoubleptrarg': False,
                'isexternal': True
            }
        }

        self.run_pass_test("bin/divisible.bc", expected)

    def test_greetings(self):
        expected = {
            'null function': {
                'calls': ['english', 'french', 'german',
                          'main', 'puts', 'spanish'], '#uses': 0,
                'calledby': [], 'hassingleptrarg': False,
                'hasdoubleptrarg': False, 'isexternal': True},
            'main': {
                'calls': ['english'], 'calledby': [], '#uses': 1,
                'hassingleptrarg': False, 'hasdoubleptrarg': False,
                'isexternal': False},
            'english': {
                'calls': ['puts'], 'calledby': ['main'], '#uses': 2,
                'hassingleptrarg': False, 'hasdoubleptrarg': False,
                'isexternal': False},
            'french': {
                'calls': ['puts'], 'calledby': [], '#uses': 1,
                'hassingleptrarg': False, 'hasdoubleptrarg': False,
                'isexternal': False},
            'german': {
                'calls': ['puts'], 'calledby': [], '#uses': 1,
                'hassingleptrarg': False, 'hasdoubleptrarg': False,
                'isexternal': False},
            'spanish': {
                'calls': ['puts'], 'calledby': [], '#uses': 1,
                'hassingleptrarg': False, 'hasdoubleptrarg': False,
                'isexternal': False},
            'puts': {
                'calls': [],
                'calledby': ['english', 'french', 'german', 'spanish'],
                '#uses': 5, 'hassingleptrarg': True,
                'hasdoubleptrarg': False, 'isexternal': True}
        }

        self.run_pass_test("bin/greetings.bc", expected)
