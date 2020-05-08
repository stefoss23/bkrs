
#include <assert.h>

#include <vector>
#include <memory>

#include <radsim/utils/timer.hpp>
#include <radsim/utils/utils.hpp>

#include <radsim/mathematics/constants.hpp>

#include <radsim/radar/target.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>
#include <radsim/radar/radar.hpp>
#include <radsim/radar/radar_interface.hpp>

using namespace std;
using namespace radsim;


void run_simulator(const string& config_file) {

  double max_time = 4.0; //s
  int num_targets = 20;
  auto config = RadarConfigParser().parseFile(config_file);
  TargetCollection targets;
  for (int i = 0; i < num_targets; i++) {
    VectorApproxFunction path( {0, max_time}, { {1000.0 * i, 0, 0}, {1000.0 * i, 100.0, 0} } );
    targets.emplace_back( move(path), 10.0 );
  }
  
  RadarInterface com(config, move(targets));
  com.setStatistics(true);

  double sim_time = 0; //s
  Timer timer;
  com.start();

  while (sim_time < max_time) {
    sim_time = com.getSimTime();
    if (com.dataReady())
      com.getData();

  }
  double time_elapsed = timer.elapsed();
  com.stop();

  cout << "Elapsed time(s)   : " << time_elapsed << endl;
  cout << "Simulation time(s): " << sim_time << endl;
  assert( double_equal(max_time, time_elapsed, 2e-1) );

}





int main(int argc , char ** argv) {

  const string config_file = string(argv[1]) + "/radar_configs/naval_radar.txt";
  run_simulator(config_file);

  return 0;
}


