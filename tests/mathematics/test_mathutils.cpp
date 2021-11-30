#include <math.h>
#include <iostream>

#include <radsim/utils/assert.hpp>

#include <radsim/mathematics/constants.hpp>
#include <radsim/mathematics/mathutils.hpp>

using namespace radsim;

void test_Rayleigh() {
  double Q = 0;
  assertDoubleEqual( rayleighPDF(2.0, Q), 0.0, 1e-4 );
  Q = 0.5;
  assertDoubleEqual( rayleighPDF(2.0, Q), 1.3863, 1e-4 );
}

void test_radial() {
  double theta = 3 * pi + 0.24;
  setRadDefaultRange(theta);
  assertTrue( theta >= 0 && theta < (2 * pi) );

  theta = 0.44;
  setRadDefaultRange(theta);
  assertTrue( theta >= 0 && theta < (2 * pi) );

}


int main() {
  test_Rayleigh();
  test_radial();
  return 0;
}
