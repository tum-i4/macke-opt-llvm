import unittest
import subprocess


class TestSimpleKTestTool(unittest.TestCase):

    def run_pass_test(self, ktestfile):

        out = subprocess.check_output(
            ["./bin/SimpleKTestTool", ktestfile], stderr=subprocess.STDOUT)

        return out.decode("utf-8")

    def test_zero(self):
        out = self.run_pass_test("ktest/i=0.ktest")
        self.assertTrue("i = 0x00000000" in out)

    def test_minusone(self):
        out = self.run_pass_test("ktest/i=-1.ktest")
        self.assertTrue("i = 0xffffffff" in out)

    def test_fortytwo(self):
        out = self.run_pass_test("ktest/i=42.ktest")
        self.assertTrue("i = 0x2a000000" in out)

    def test_hi(self):
        out = self.run_pass_test("ktest/str=Hi.ktest")
        self.assertTrue("str = 0x486901ff" in out)

    def test_singleStructPointer(self):
        out = self.run_pass_test("ktest/p=struct.ktest")
        self.assertTrue(
            "p = 0x0100000065"
            "ffffffffffffffffffffffffffffffffffffffffffffffffffffff" in out)

    def test_twoIntegers(self):
        out = self.run_pass_test("ktest/a=1,b=0.ktest")
        self.assertTrue("a = 0x01000000" in out)
        self.assertTrue("b = 0x00000000" in out)
