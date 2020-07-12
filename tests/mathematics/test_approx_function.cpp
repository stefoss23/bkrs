#include <assert.h>
#include <iostream>
#include <complex>
#include <vector>

#include <radsim/utils/utils.hpp>
#include <radsim/utils/test_utils.hpp>

#include <radsim/mathematics/math_vector.hpp>
#include <radsim/mathematics/approx_function.hpp>

using namespace std;
using namespace std::complex_literals;
using namespace radsim;

void test_simple() {

  std::vector<double> entry = {0.0, 1.0};
  std::vector<double> value = {2.0, 4.0};
  std::vector<double> entry_copy = entry;
  std::vector<double> value_copy = value;

  double * entry_ptr = entry.data();
  double * value_ptr = value.data();

  ApproxFunction<double> func( move(entry), move(value) );
  assert( entry.size() == 0 );
  
  assert( double_equal(func.output(0.3), 2.6, 1e-4));
  assert( double_equal(func.output(-0.5), 2.0, 1e-4));
  assert( double_equal(func.output(8.0), 4.0, 1e-4));

  assert( entry_ptr == func.getEntryVector().data() );
  assert( value_ptr == func.getValueVector().data() );

  ApproxFunction<double> copy = func;
  assert( entry_copy == copy.getEntryVector() );
  assert( value_copy == copy.getValueVector() );

  assert( entry_ptr == func.getEntryVector().data() );
  assert( value_ptr == func.getValueVector().data() ); 

  vector<double> input = {-0.5, 8.0};
  vector<double> result = {2.0, 4.0};

  assert( copy.outputVector(input) == result );

  ApproxFunction<double> func_1( {3, 7}, {7, 8} ); 

  copy = func_1;

  assert( func_1.getEntryVector() == copy.getEntryVector() );
  assert( func_1.getValueVector() == copy.getValueVector() );
  assert( double_equal(copy.output(5.0), 7.5, 1e-4) );
}


void use_moved_function() {
  std::vector<double> entry = {0.0, 1.0};
  std::vector<double> value = {2.0, 4.0};
  ApproxFunction<double> func( move(entry), move(value) );
  auto copy = move(func);
  func.output(8);
}


void test_move() {

  std::vector<double> entry = {0.0, 1.0};
  std::vector<double> value = {2.0, 4.0};
  double * entry_ptr = entry.data();
  double * value_ptr = value.data();

  ApproxFunction<double> func( move(entry), move(value) );
  assert( entry_ptr == func.getEntryVector().data() );
  assert( value_ptr == func.getValueVector().data() );

  ApproxFunction<double> copy = move(func);
  assert( entry_ptr == copy.getEntryVector().data() );
  assert( value_ptr == copy.getValueVector().data() );

  assert( double_equal(copy.output(-1), 2.0, 1e-3) );
  assert( double_equal(copy.output(0.5), 3.0, 1e-3) );
  assert( double_equal(copy.output(2.0), 4.0, 1e-3) );
  
}


void use_elsewhere(const ApproxFunction<double>& func) {
  cout << "Size = " << func.getEntryVector().size() << endl;
}


ApproxFunction<double> function(double ** entry_ptr, double ** value_ptr) {
  std::vector<double> entry = {0.0, 1.0};
  std::vector<double> value = {2.0, 4.0};
  *entry_ptr = entry.data();
  *value_ptr = value.data();
  
  ApproxFunction<double> func( move(entry), move(value) );
  use_elsewhere( func );
  return func;
}


void test_return() {
  double * entry_ptr;
  double * value_ptr;
  ApproxFunction<double> func = function(&entry_ptr, &value_ptr);
  assert( func.getEntryVector().data() == entry_ptr );
  assert( func.getValueVector().data() == value_ptr );
}

void test_complex() {
  std::vector<double> entry = {0.0, 1.0, 2.0};
  std::vector<std::complex<double> > value = {-1.0+(0.0i), 
                                               2.0+(1.0i),
                                               3.0+(3.0i)};
  ApproxFunction<complex<double>> func(entry, value);
  assert( complex_equal(func.output(1.4), 2.4+(1.8i), 1e-4));
  const std::vector<double>& entry_copy     = func.getEntryVector();
  const vector<complex<double>>& value_copy = func.getValueVector();
  assert( entry_copy.size() == 3);
  assert( value_copy.size() == 3);
  assert( entry[0] == entry_copy[0] );
  assert( value[0] == value_copy[0] );
}

void test_a() {
  vector<double> entry                = {0.0,        2.0,        4.0};
  vector<std::complex<double> > value = {1.0+(0.0i), 2.0+(0.0i), 3.0+(0.0i)};
  ApproxFunction<complex<double>> func(entry, value);
  assert( complex_equal(func.output(1.0), 1.5, 1e-5));
  assert( complex_equal(func.output(3.0), 2.5, 1e-5));

}

void test_constant() {
  ApproxFunction<double> func(double(2.0));
  assert( func.output(8.0) == 2.0 );
  assert( func.output(0.5) == 2.0 );
  assert( func.output(1.0) == 2.0 );
  auto copy = func;
  assert( copy.output(0) == 2.0 );
}

void test_segment() {
  vector<double> entry = {0, 1};
  vector<double> value = {1, 1};
  double * entry_ptr = entry.data();
  double * value_ptr = value.data();
  ApproxFunction<double> func(move(entry), move(value), 0, 2);
  assert( func.output(-0.0001) == 0 );
  assert( func.output(0.0001) == 1 );
  assert( func.output(0.9999) == 1 );
  assert( func.output(1.0001) == 2);
  assert( func.getEntryVector().data() == entry_ptr );
  assert( func.getValueVector().data() == value_ptr );
}

void test_math_vector() {
  math_vector v1 = {1, 2, 4};
  math_vector v2 = {2, 4, 8};
  ApproxFunction<math_vector> func({0, 1}, (vector<math_vector>){v1, v2});
  math_vector g1 = func.output(0.5); 
  math_vector g1_c = {1.5, 3.0, 6.0};
  assert( g1 == g1_c );
  math_vector g0 = func.output(-1.0);
  assert( g0 == v1 );
  
}

DoubleApproxFunction return_copy(const DoubleApproxFunction& orig, const double ** entry_ptr, const double ** value_ptr) {
  *entry_ptr = orig.getEntryVector().data();
  *value_ptr = orig.getValueVector().data();
  return orig;
}

void test_func_copy() {
  DoubleApproxFunction f(4.4);
  const double * entry_ptr;
  const double * value_ptr;
  DoubleApproxFunction * ptr = &f;
  const DoubleApproxFunction& copy = return_copy(f, &entry_ptr, &value_ptr);
  assert( &copy != ptr );
  assert( entry_ptr != copy.getEntryVector().data() );
  assert( value_ptr != copy.getValueVector().data() );
}

void wrong_1() {
  std::vector<double> entry = {1.0};
  std::vector<double> value = {7.0};
  ApproxFunction<double> func(entry, value);
}

void wrong_2() {
  std::vector<double> entry = {0.0, 1.0};
  std::vector<double> value = {2.0};
  ApproxFunction<double> func(entry, value);
}

void wrong_3() {
  std::vector<double> entry = {1.0, 1.0};
  std::vector<double> value = {2.0, 4.0};
  ApproxFunction<double> func(entry, value);
}

void wrong_4() {
  std::vector<double> entry = {0.0, 1.0};
  std::vector<int> value = {2, 4};
  ApproxFunction<int> func(entry, value);
}

int main() {

  test_simple();
  test_move();
  test_return();
  test_complex();
  test_a();
  test_constant();
  test_segment();
  test_math_vector();

  assert_throw<std::invalid_argument>( &wrong_1 );
  assert_throw<std::invalid_argument>( &wrong_2 );
  assert_throw<std::invalid_argument>( &wrong_3 );
  assert_throw<std::invalid_argument>( &wrong_4 );
  assert_throw<std::logic_error>( &use_moved_function );

  return 0;
}
