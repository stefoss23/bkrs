
import numpy as np
import unittest

from bkradsim.mathematics import DoubleApproxFunction
from bkradsim.mathematics import ComplexApproxFunction
from bkradsim.mathematics import MathVectorApproxFunction

class TableFunctionTester(unittest.TestCase):

  def test_double_table(self):
     entry = np.array([ 0.0, 2.0, 4.0])
     value = np.array([-2.0, 2.0, 6.0])
     T = DoubleApproxFunction(entry, value)
     self.assertEqual(4.0, T(3.0))
     self.assertEqual(0.0, T(1.0))
     assert( isinstance(T(2.0), float) )
     assert(  len( T(np.array([0.0, 1.0])) ) == 2 )
     T = DoubleApproxFunction(entry, value, 6, 60)
     assert( T(1000) == 60)

  def test_double_table_1(self):
      entry = np.array([ 0.0, 2.0, 4.0])
      value = np.array([ 1.0, 2.0, 3.0])
      T = DoubleApproxFunction(entry, value)
      self.assertEqual(1.5, T(1.0))
      self.assertEqual(2.5, T(3.0))

  def test_complex_table(self):
     entry = np.array([0.0     , 2.0     , 4.0])
     value = np.array([1.0+0.0j, 2.0+2.0j, 3.0+4.0j])
     T = ComplexApproxFunction(entry, value)
     self.assertEqual(1.5+1j, T(1.0))
     self.assertEqual(2.5+3j, T(3.0))
     v = np.array([1.0, 3.0])
     result = T(v)
     self.assertEqual(   result[0], 1.5+1j   )
     self.assertEqual(   result[1], 2.5+3j   )
     entry_copy = T.entry
     value_copy = T.value
     assert(not isinstance(entry_copy[0], complex))
     self.assertEqual(   len(entry_copy), 3)
     self.assertEqual(   len(value_copy), 3)
     self.assertEqual(   entry_copy[0], entry[0])
     self.assertEqual(   value_copy[0], value[0])
     assert( isinstance(T(0), complex) )
     T = ComplexApproxFunction(entry, value, 5, 5)
     assert( T(1000) == 5)

  def test_math_vector_approx(self):
     entry = np.array([0.0 , 2.0])
     value = np.array([  [0.0, 0.0, 0.0], [1.0, 2.0, 3.0]   ])
     F = MathVectorApproxFunction(entry, value)
     self.assertTrue( np.array_equal(np.array([0.5, 1.0, 1.5]), F(1.0)  ) )
     entry_copy = F.entry
     value_copy = F.value
     self.assertTrue(np.array_equal( entry, entry_copy ) )
     self.assertTrue(np.array_equal( value, value_copy ) )
     shape = F(0).shape
     assert( shape[0] == 3 )
     shape = F( np.array([1.0, 2.0]) ).shape
     assert( shape[0] == 2 )

    

if __name__ == '__main__':
    unittest.main()
