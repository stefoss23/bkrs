/* This is the first test of real time processing with rads. */

#include <vector>
#include <memory>

#include <radsim/mathematics/constants.hpp>

#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>
#include <radsim/radar/radar.hpp>
#include <radsim/radar/radar_interface.hpp>

using namespace std;
using namespace radsim;


void run_simulator(const string& filename) {
  double dt = 0.03; //s
  auto config = RadarConfigParser().parseFile(filename);
  RadarInterface com(config, {}, dt);
  com.start();
  com.stop();

}





int main(int argc , char ** argv) {

  const string config_file = string(argv[1]) + "/radar_configs/short_range_radar.txt";
  run_simulator(config_file);

  return 0;
}


