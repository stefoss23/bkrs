

import unittest
import numpy as np

from test_devel_python import *

class TestVectors(unittest.TestCase):

    def test_double(self):
        v = GetDoubleArray()
        assert (isinstance(v, np.ndarray))
        assert (len(v) == 3)
        assert (isinstance(v[0], np.float64))
        assert (v[0] == 0.25)
        assert (v[1] == 0.125)
        assert (v[2] == 0.5)

    def test_send_double(self):
        v = np.array([0.625, 0.125, 0.5])
        SendDoubleArray(v)

    def test_empty_double(self):
        v = np.array([])
        SendEmptyDoubleArray(v)

    def test_wrong_double(self):
        with self.assertRaises(TypeError):
            SendDoubleArray('text')

    def test_send_ushort(self):
        SendUShortArray(np.array([65536, 5]))


    def test_complex(self):
        v = GetComplexArray()
        assert (isinstance(v, np.ndarray))
        assert (len(v) == 5)
        assert (isinstance(v[0], np.complex128))
        assert (v[0] == 1.50 + 2.750j);
        assert (v[1] == 1.25 + 4.500j);
        assert (v[2] == 1.00 + 8.250j);
        assert (v[3] == 0.25 + 6.125j);
        assert (v[4] == 8.00 - 9.875j);


if __name__ == '__main__':
    unittest.main()
