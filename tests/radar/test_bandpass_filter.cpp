#include <radsim/utils/assert.hpp>

#include <radsim/radar/bandpass_filter.hpp>


using namespace std;
using namespace radsim;

void test_standard() {
  auto H = createBandpassFilter(BandpassFilter::Standard, 2.0);
  assertComplexEqual( H.output( 0.00), 1.0, 1e-3 );
  assertComplexEqual( H.output( 0.50), 1.0, 1e-3 );
  assertComplexEqual( H.output( 1.00), 0.5, 1e-3 );
  assertTrue( abs(H.output( 1.50)) < 1e-3 );
  assertComplexEqual( H.output(-0.50), 1.0, 1e-3 );
  assertComplexEqual( H.output(-1.00), 0.5, 1e-4 );
  assertTrue( abs(H.output(-1.50)) < 1e-3 );
  assertComplexEqual( H.output( 1.25), 0.14644, 1e-3 );
}

void test_rectangular() {
  auto H = createBandpassFilter(BandpassFilter::Rectangular, 2.0);
  assertComplexEqual( H.output( 0.00), 1.0, 1e-3 );
  assertComplexEqual( H.output( 0.99), 1.0, 1e-3 );  
  assertTrue( abs(H.output( 1.01)) < 1e-3 );

}

int main() {
  test_standard();
  test_rectangular();
  return 0;
}
