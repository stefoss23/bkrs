#!/usr/bin/python3

import unittest
import numpy as np

from bkradsim.radar import Target, TargetCollection
from bkradsim.mathematics import MathVectorApproxFunction

class TestTargetType(unittest.TestCase):

    def test_target(self):
        T = Target(np.array([2.0, 2.0, 1.0]), 2.0)
        v = np.array([2.0, 2.0, 1.0])
        assert( np.array_equal(T.get_position(), v) )
        assert( T.rcs == 2.0 )
        T.set_position(np.array([1.0, 1.0, 1.0]))
        assert( np.allclose(T.get_position(), np.array([1.0, 1.0, 1.0])) )


        entry = np.array([0.0, 2.0])
        value = np.array([[0.0, 0.0, 0.0], [1.0, 2.0, 3.0]])
        Path = MathVectorApproxFunction(entry, value)
        T = Target(Path, 4.0)
        v = np.array([0.5, 1.0, 1.5])
        assert( np.array_equal(T.get_position(1.0), v  ))
        assert( T.rcs == 4.0 )
        path_copy = T.get_path()
        assert( len(path_copy.entry) == 2 )

    def test_collection(self):
        pos1 = np.array([1.0, 2.0, 4.0])
        T1 = Target(pos1, 2.0)

        pos2 = np.array([2.5, 1.5, 8.0])
        T2 = Target(pos2, 3.0)

        collection = TargetCollection()
        assert(collection.size == 0)
        collection.append_copy(T1);
        collection.append_copy(T2);
        assert(collection.size == 2)
        assert( collection.iget_copy(0).rcs == 2.0 )
        assert( collection.iget_copy(1).rcs == 3.0 )
        collection.idelete(0)
        assert(collection.size == 1)
        assert( collection.iget_copy(0).rcs == 3.0 )
        collection.clear()
        assert(collection.size == 0)        


if __name__ == '__main__':
    unittest.main()
