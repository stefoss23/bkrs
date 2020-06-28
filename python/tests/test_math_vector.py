#!/usr/bin/python3

import unittest
import numpy as np

from test_devel_python import *

class TestMathVector(unittest.TestCase):

    def test_receive(self):
        v = GetMathVector()
        assert(len(v) == 3)
        assert (isinstance(v[0], np.float64))
        assert (v[0] == 1.125)
        assert (v[1] == 4.750)
        assert (v[2] == 0.625)

    def test_send_around(self):
        v1 = np.array([1.250, 4.625, 8.000])
        v2 = SendAroundMathVector(v1)
        assert(len(v2) == 3)
        assert (isinstance(v2[0], np.float64))
        assert (v2[0] == 2.25)
        assert (v2[1] == 8.625)
        assert (v2[2] == 16.0)

    def test_get_array(self):
        mtrix = GetVectorMathVector();
        assert(len(mtrix) == 2)
        assert(isinstance(mtrix[0, 0], np.float64))
        assert(mtrix[0, 0] == 1.50)
        assert(mtrix[0, 1] == 2.75)
        assert(mtrix[0, 2] == 4.25)
        assert(mtrix[1, 0] == 3.25)
        assert(mtrix[1, 1] == 6.75)
        assert(mtrix[1, 2] == 8.00)

    def test_send_array(self):
        mtrix = np.zeros([2, 3])
        mtrix[0, 0] = 4.50
        mtrix[0, 1] = 1.75
        mtrix[0, 2] = 2.25
        mtrix[1, 0] = 6.25
        mtrix[1, 1] = 8.75
        mtrix[1, 2] = 2.00
        SendVectorMathVector(mtrix)

if __name__ == '__main__':
    unittest.main()
