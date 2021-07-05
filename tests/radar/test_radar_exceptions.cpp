#include <memory>
#include <exception>

#include <radsim/utils/assert.hpp>

#include <radsim/radar/radar_config.hpp>


using namespace std;
using namespace radsim;

void wrong_1() {
  RadarConfig config ;
  config.assertParametersSet();
}



int main() {
  assertThrow( wrong_1(), exception );
}
