
#include <exception>
#include <complex>

#include <radsim/utils/assert.hpp>

using namespace std;
using namespace radsim;

//These tests need to be checked manually also, since
//all failed assertions causes immediate program abortion. 

void test_assert_double() {
  assertDoubleEqual(1.0, 1.0, 1e-4);
}

void test_assert_complex() {
  assertComplexEqual(-2.0+(1.0i), -2.0+(1.0i), 1e-4);
}

void test_assert_int() {
  assertIntEqual(2, 2);
}

void test_assert_string() {
  assertStringEqual("a", "a");
}

void test_assert_true_false() {
  assertTrue(true);
  

  assertFalse(false);
}


void throw_error() {
  throw logic_error("");
}

void test_assert_throw() {
  assertThrow( throw_error(), invalid_argument );
}


int main() {

  test_assert_double();
  test_assert_complex();
  test_assert_int();
  test_assert_string();
  test_assert_true_false();

  test_assert_throw();

  return 0;
}
