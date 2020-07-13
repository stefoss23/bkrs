#include <iostream>
#include <assert.h>
#include <exception>
#include <complex>

#include <radsim/utils/utils.hpp>
#include <radsim/utils/test_utils.hpp>

using namespace std::complex_literals;
using namespace radsim;

void test_equal_double() {
  assert( double_equal(50.0, 50.1, 1e-2) );
  assert(!double_equal(50.0, 52.1, 1e-2) );
}

void test_equal_negative() {
  assert( double_equal(- 2.0,  -2.0,  1e-2) );
  assert( double_equal(- 2.01, -2.00, 1e-2) );
}

void test_equal_complex() {
  std::complex<double> c1 = 2.00012 + (1.0000155i);
  std::complex<double> c2 = 2.0 + (1.0i);
  assert( complex_equal(c1, c2, 1e-3) );
  assert(!complex_equal(c1, c2, 1e-13));
  c1 = -2.0+(1.0i);
  assert(!complex_equal(c1, c2, 1e-3));
}

void do_throw() {
  throw std::invalid_argument("GG");
}

void test_assert_throw() {
  assert_throw<std::invalid_argument>(&do_throw);
}


int main() {
  test_equal_double();
  test_equal_negative();
  test_equal_complex();
  test_assert_throw();
  return 0;
}
