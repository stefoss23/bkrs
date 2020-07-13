#include <assert.h>
#include <math.h>

#include <iostream>

#include <radsim/utils/utils.hpp>

#include <radsim/mathematics/math_vector.hpp>
#include <radsim/mathematics/constants.hpp>

using namespace std;
using namespace radsim;

void test_1() {
  double deg_to_rad = pi / 180.0;
  double rad_to_deg = 1 / deg_to_rad;

  math_vector a = {2, 2, 1};
  assert( double_equal(math_vector_length(a), 3, 1e-4) );
  math_vector b = {1, 2, 3};
  assert( double_equal(a * b, 9, 1e-4) );

  a = {1, 0, 0};
  b = {0, 1, -1};
  assert( double_equal(math_vector_angle(a, b), 0.5 * pi, 1e-4) );

  b = {-1, 1, 0}; 
  assert( double_equal(math_vector_angle(a, b), 0.75 * pi, 1e-4 ) );

  a = {1.5, 2.25, 8.75};
  assert( ((a / 2.0) == (math_vector){0.75, 1.125, 4.375}) );

  a = math_vector_unit({3, 4, 0});
  assert( double_equal(a[0], 0.6, 1e-4) );
  assert( double_equal(a[1], 0.8, 1e-4) );
  assert( a[2] == 0 );

  a = math_vector_unit({45, 789, 11});
  assert( double_equal(math_vector_length(a), 1.0, 1e-4) );

  a = (math_vector){1.0, 2.0, 4.0} * 2.0;
  b = {2.0, 4.0, 8.0};
  assert( a == b );
  a = 2.0 * (math_vector){1.0, 2.0, 4.0};
  assert( a == b );

  math_vector c = (math_vector){1.0, 0.5, 0.25} + (math_vector){0.5, 1.5, 0.75};
  math_vector d = {1.5, 2.0, 1.0};
  assert( c == d ); 

  a = {1, 4, 8};
  double * ptr1 = a.data();
  b = {0.5, 2.0, 4.0};
  a /= 2.0;
  double * ptr2 = a.data();
  assert( a == b );
  assert( ptr1 == ptr2 );

  a = {2, 2, 1};
  ptr1 = a.data();
  math_vector_set_unit(a);
  assert( double_equal(a[0] , 0.666667, 1e-3) );
  assert( double_equal(a[1] , 0.666667, 1e-3) );
  assert( double_equal(a[2] , 0.333333, 1e-3) );
  ptr2 = a.data();
  assert( ptr1 == ptr2 );

  a = (math_vector){2, 2, 1} - (math_vector){3, 2, 5};
  b = {-1, 0, -4};
  assert( a == b );
}


void test_transform() {
  double deg_to_rad = pi / 180.0;
  math_vector v  = {2, 0, 0};
  math_vector t1 = {cos(30 * deg_to_rad), sin(30 * deg_to_rad), 0};
  math_vector_rotate_xy(v, 30 * deg_to_rad); math_vector_set_unit(v);
  assert( v == t1 );
  math_vector t2 = {0, -1, 0};
  math_vector_rotate_xy(t1, -120 * deg_to_rad);
  assert( math_vector_equal(t1, t2, 1e-4) );
  math_vector t3 = {0.707, 0.707, 0};
  math_vector_rotate_xy(t2, 135 * deg_to_rad);
  assert( math_vector_equal(t2, t3, 1e-2) );
}

void test_spherical() {
  math_vector x = {2, 0, 0};
  math_vector y = {0, 3, 0};
  math_vector z = {0, 0, 6};
  auto x_ = math_vector_spherical(2, 0, 0);
  auto y_ = math_vector_spherical(3, 0, pi/2);
  auto z_ = math_vector_spherical(6, pi/2, 8);
  assert( math_vector_equal(x_, x, 1e-4) );
  assert( math_vector_equal(y_, y, 1e-4) );
  assert( math_vector_equal(z_, z, 1e-4) );
}

int main() {
  test_1();
  test_transform();
  test_spherical();
  return 0;
}
