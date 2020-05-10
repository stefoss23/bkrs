#include <iostream>
#include <string>
#include <vector>
#include <radsim/mathematics/math_vector.hpp>
#include <radsim/radar/target.hpp>
#include <radsim/radar/radar_interface.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>

using namespace std;
using namespace radsim;


int main() {
  RadarConfigParser parser;
  auto config = parser.parseFile("naval_radar.txt");

  TargetCollection targets;
  targets.emplace_back( (math_vector){0, 5000, 0}, 2 );

  RadarInterface radar_if(config, targets);

  radar_if.start();
  int count = 0;
  while (count <= 10) {
    if (radar_if.dataReady()) {
      auto pulse_data = radar_if.getData();

      cout << "t(s): " << pulse_data.getStartTime() << ": ";
      for (size_t i = 250; i < 260; i++)
        cout << pulse_data.registry[i] << ", ";
      cout << endl;

      count++;
    }      
  }
  cout << radar_if.getSimTime();
  radar_if.stop();

  return 0;
}
