#include <iostream>
#include <exception>
#include <complex>

#include <radsim/utils/assert.hpp>
#include <radsim/utils/utils.hpp>
#include <radsim/utils/test_utils.hpp>

using namespace std::complex_literals;
using namespace radsim;

void test_equal_double() {
  assertTrue( double_equal(50.0, 50.1, 1e-2) );
  assertTrue(!double_equal(50.0, 52.1, 1e-2) );
}

void test_equal_negative() {
  assertTrue( double_equal(- 2.0,  -2.0,  1e-2) );
  assertTrue( double_equal(- 2.01, -2.00, 1e-2) );
}

void test_equal_complex() {
  std::complex<double> c1 = 2.00012 + (1.0000155i);
  std::complex<double> c2 = 2.0 + (1.0i);
  assertTrue( complex_equal(c1, c2, 1e-3) );
  assertTrue(!complex_equal(c1, c2, 1e-13));
  c1 = -2.0+(1.0i);
  assertTrue(!complex_equal(c1, c2, 1e-3));
}



int main() {
  test_equal_double();
  test_equal_negative();
  test_equal_complex();
  return 0;
}
