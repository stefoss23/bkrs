
from bkradsim.radar import ADC, ADCMode

import numpy as np
import unittest

class ADCTester(unittest.TestCase):

    def test1(self):
        adc = ADC(4, ADCMode.Power, 67.2)
        adc = ADC(4, ADCMode.Logarithm, 67.2, 100.0)
        self.assertEqual(16, adc.num_levels)

    def test2(self):
        adc = ADC(10, ADCMode.Power, 1e-12)
        self.assertEqual(adc.convert(1e-12 * 100.001), 100)
        with self.assertRaises(Exception):
            adc.Convert('sdfs')


if __name__ == '__main__':
    unittest.main()
