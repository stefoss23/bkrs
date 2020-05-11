#include <iostream>
#include <string>
#include <vector>
#include <radsim/radar/radar.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>

using namespace std;
using namespace radsim;

void displayPulseData(const PulseData& pulse_data) {
  auto& reg = pulse_data.registry;
  for (size_t i = 0; i < 10; i++)
    cout << "--- sampling " << i << ": " << reg[i] << endl;
}


int main() {
   RadarConfigParser parser;
   string config_file = "stationary_radar.txt";
   RadarConfig config = parser.parseFile(config_file);
   Radar radar(config);

   cout << "Sim time(s): " << radar.getCurrentTime() << endl;
   auto pulse_data = radar.generatePulseData();
   cout << "Sim time(s): " << radar.getCurrentTime() << endl;
   pulse_data = radar.generatePulseData();
   cout << "Sim time(s): " << radar.getCurrentTime() << endl;
   displayPulseData(pulse_data);
   
   return 0;
}
