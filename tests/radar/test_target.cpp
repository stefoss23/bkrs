#include <iostream>
#include <assert.h>

#include <memory>

#include <radsim/utils/utils.hpp>

#include <radsim/mathematics/math_vector.hpp>
#include <radsim/mathematics/approx_function.hpp>

#include <radsim/radar/target.hpp>

using namespace std;
using namespace radsim;

void test_get_pos() {
  math_vector pos = {20, 20, 10}; //m
  Target target(pos, 8.0);
  assert( double_equal(math_vector_length(target.getPosition()), 30, 1e-4) );
  assert( target.getRCS() == 8.0 );

  target.setPosition({3, 4, 0});
  assert( double_equal(math_vector_length(target.getPosition()), 5, 1e-4 ) ); 
}

void test_path() {
  math_vector start = {0, 0, 0};
  math_vector end   = {1, 2, 3};

  VectorApproxFunction path({0, 1}, vector<math_vector>{start, end});
  Target target(move(path), 8.0);

  math_vector middle = {0.5, 1.0, 1.5};
  assert( target.getPosition(0) == start );

  assert( target.getPosition(0.5) == middle );
}

void test_move() {
  math_vector start = {0, 0, 0};
  math_vector end   = {1, 2, 3};

  VectorApproxFunction path({0, 1}, vector<math_vector>{start, end});

  Target target(move(path), 8.0);  
  const void * ptr_entry = target.getPath().getEntryVector().data();
  const void * ptr_array = target.getPath().getValueVector()[0].data();

  Target copy = move(target);
  assert( ptr_entry == copy.getPath().getEntryVector().data() );
  assert( ptr_array == copy.getPath().getValueVector()[0].data() );

}


int main() {
  test_get_pos();
  test_path();
  test_move();
}
