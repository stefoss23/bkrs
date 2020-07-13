#include <assert.h>

#include <memory>
#include <exception>

#include <radsim/utils/utils.hpp>
#include <radsim/utils/test_utils.hpp>

#include <radsim/radar/radar_config.hpp>


using namespace std;
using namespace radsim;

void wrong_1() {
  RadarConfig config ;
  config.assertParametersSet();
}



int main() {
  assert_throw<exception>( &wrong_1 );
}
