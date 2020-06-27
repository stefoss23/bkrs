import unittest

from bkradsim.mathematics import *

class MathUtilsTester(unittest.TestCase):

   def test_math_utils(self):
      rayleigh(8, 0.5)
      power_to_amp(8)

if __name__ == '__main__':
    unittest.main()
