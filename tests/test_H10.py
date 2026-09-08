import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "host"))
import h10_cli as m


class TestKeymap(unittest.TestCase):
    def test_lowercase(self):
        mod, key = m.map_char("a")
        self.assertEqual(mod, 0)
        self.assertEqual(key, 0x04)

    def test_uppercase_shift(self):
        mod, key = m.map_char("A")
        self.assertEqual(mod, 0x02)
        self.assertEqual(key, 0x04)

    def test_special_space(self):
        mod, key = m.map_char(" ")
        self.assertEqual(key, 0x2C)

    def test_unsupported(self):
        self.assertIsNone(m.map_char("\x00"))


if __name__ == "__main__":
    unittest.main()
