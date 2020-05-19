import unittest

import bkradsim.utils as Utils


class UtilsTester(unittest.TestCase):

    def test_complex_equal(self):
        x = 1.01 + 2.0j
        y = 1.00 + 2.0j
        self.assertTrue(Utils.complex_equal(x, y, 1e-2))
        self.assertFalse(Utils.complex_equal(x, y, 1e-5))

if __name__ == '__main__':
    unittest.main()
