import numpy as np
import unittest

import bkradsim.utils as Utils

from bkradsim.mathematics import ComplexApproxFunction
from bkradsim.mathematics import Fourier

def Gauss(t):
    return np.exp(-t * t)

def Dirac(vector):
    width = 0.001
    result = np.zeros(len(vector), dtype=np.float64)
    for index, t in enumerate(vector):
        if ((t - 0.5) <= width and (t - 0.5) >= -width):
            result[index] = 0.5 / width
    return result

class FourierTester(unittest.TestCase):

    #Transform of Gauss(t) yields exp(- PI² * f²) * sqrt(PI)
    def test_Gauss(self):
        self.assertTrue( Utils.complex_equal( Fourier.Transform(Gauss, 0, 4e-4, -3.0, 3.0), np.sqrt(np.pi), 1e-2 ) )
        f = np.array([-0.5, 0.0, 0.5])
        F = Fourier.Transform(Gauss, f, 4e-4, -3.0, 3.0)
        self.assertTrue(Utils.complex_equal( F(0.0), np.sqrt(np.pi), 2e-3  ))
        self.assertTrue(Utils.complex_equal( F(0.499999), np.exp(-0.25 * np.pi * np.pi) * np.sqrt(np.pi), 2e-3  ))

    #Transform of Dirac yields F = exp(- pi * i * f)
    #The inverse yields        F = exp(  pi * i * f)
    def test_Dirac(self):
        self.assertTrue(Utils.complex_equal(Fourier.Transform(Dirac, 0.0, 4e-4, -3.0, 3.0),  1.0,  1e-2))
        self.assertTrue(Utils.complex_equal(Fourier.Transform(Dirac, 0.5, 4e-4, -3.0, 3.0), -1.0j, 1e-2))
        self.assertTrue(Utils.complex_equal(Fourier.Transform(Dirac, 0.5, 4e-4, -3.0, 3.0, True), 1.0j, 1e-2))

    def test_tablefunction(self):
        pass

if __name__ == '__main__':
    unittest.main()
