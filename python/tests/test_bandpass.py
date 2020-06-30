import numpy as np
import unittest

from bkradsim.mathematics import ComplexApproxFunction
from bkradsim.radar import BandpassFilter, create_bandpass_filter


class BandPassTester(unittest.TestCase):

   def test_bandpass(self):
      standard = create_bandpass_filter(BandpassFilter.Standard, 1e-6)
      assert( isinstance(standard, ComplexApproxFunction) )
      rect = create_bandpass_filter(BandpassFilter.Rectangular, 1e-5)
      assert( isinstance(rect, ComplexApproxFunction) )

if __name__ == '__main__':
    unittest.main()



