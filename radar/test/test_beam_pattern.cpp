#include <assert.h>

#include <iostream>

#include <radsim/utils/utils.hpp>

#include <radsim/mathematics/approx_function.hpp>

#include <radsim/radar/beam_pattern.hpp>

using namespace radsim;

void test() {
 
  auto f = createBeamPattern(BeamPattern::Gaussian, 2.0);
  assert( double_equal(f.output( 1.0), 0.5, 1e-3) );
  assert( double_equal(f.output( 0.0), 1.0, 1e-3) );
  assert( double_equal(f.output(-1.0), 0.5, 1e-3) );
}


int main() {
  test();
  return 0;
}
