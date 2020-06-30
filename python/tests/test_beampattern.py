import numpy as np
import unittest

from bkradsim.mathematics import DoubleApproxFunction
from bkradsim.radar import BeamPattern, create_beam_pattern


class BeamPatternTester(unittest.TestCase):

   def test_pattern(self):
      pattern = create_beam_pattern(BeamPattern.Gaussian, 2.0)
      self.assertAlmostEqual(pattern(0.0), 1, places=4)
      self.assertAlmostEqual(pattern(1.0), 0.5, places=4)


if __name__ == '__main__':
    unittest.main()
