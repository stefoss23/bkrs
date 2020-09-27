/* This is the first test of real time processing with rads. */


#include <assert.h>

#include <vector>
#include <memory>
#include <exception>

#include <radsim/utils/test_utils.hpp>
#include <radsim/utils/timer.hpp>

#include <radsim/mathematics/math_vector.hpp>
#include <radsim/mathematics/constants.hpp>

#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>
#include <radsim/radar/radar.hpp>
#include <radsim/radar/radar_interface.hpp>

using namespace std;
using namespace radsim;

RadarConfig config;

void run_simulator() {

  double dt = 0.2; //s
  config.setAntRotSpeed(0); //pointing in x-direction only
  Radar radar(config);
  double base_distance = 9987.5; //m, this will be at range bin 253, should also encompass 254
  double signal_strength = 1e-11; //W, the resultant received signal from target

  double maxrange = radar.getUnAmbiguousRange(); //s

  //the target will not appear in pulse 0, but all pulses thereafter
  TargetCollection collection;
  collection.emplace_back( Target({maxrange + base_distance, 0, 0}, 1.0) );

  math_vector boresight = {1, 0, 0}; //initial pointing director of radar. 

  RadarInterface com(config, {Target({maxrange + base_distance, 0, 0}, 1.0)}, dt);
  com.setAddNoise(false);
  com.setStatistics(true);

  assert( complex_equal(com.getRange(200), radar.getRange(200), 1e-4) );
  assert( complex_equal(com.getRange(400), radar.getRange(400), 1e-4) );

  double sim_time = 0; //s

  double max_time = 4.0; //s
  double test_data_time = -1.0;
  
  Timer timer;
  com.start(true, signal_strength);

  int pulse_indx = 0;

  while (sim_time < max_time) {
    sim_time = com.getSimTime(); //s

    if (com.dataReady()) { 
      auto data = com.getData();
      assert( data.getStartTime() > test_data_time );
      test_data_time = data.getStartTime();  //s    

      vector<unsigned short> registry = move(data.registry);

      assert( registry[400] == 0 );
      assert( registry[200] == 0 );
      if (pulse_indx > 0) {
        assert( registry[253] > 100 );
        assert( registry[254] > 100 );
      }
      else {
        assert( registry[253] == 0 );  //the target is begyond unambiguous range in pulse 0
        assert( registry[254] == 0 );
      }

      assert( data.getBoresight() == boresight );
      pulse_indx++;

    }

  }
  com.stop();
  double t_elapsed = timer.elapsed(); //s

  cout << "Test time(s): " << t_elapsed << endl;

  if (! double_equal(t_elapsed, max_time, 2.0e-1) ) {
    throw logic_error(__PRETTY_FUNCTION__ + string(": too much time.") );
  }

}


void run_paused_continued() {
  RadarInterface com(config, {});
  double delta_time = 0.25;
  double max_time = 0.25;
  
  com.start();
  while (com.getSimTime() < delta_time) {
    if (com.dataReady()) com.getData();
  }
  com.stop();
  assert( !double_equal(com.getSimTime(), max_time, 1.0e-1) );

  max_time += delta_time;
  com.start();
  while (com.getSimTime() < delta_time) {
    if (com.dataReady()) com.getData();
  }
  com.stop();
  assert( !double_equal(com.getSimTime(), max_time, 1.0e-1) );

  max_time += delta_time;
  com.start();
  while (com.getSimTime() < delta_time) {
    if (com.dataReady()) com.getData();
  }
  com.stop();
  assert( !double_equal(com.getSimTime(), max_time, 1.0e-1) );      
}


void run_wrong2() {
  RadarInterface com(config, {});
  com.start();
  com.start();
}



int main(int argc , char ** argv) {

  const string config_file = string(argv[1]) + "/radar_configs/short_range_radar.txt";
  config = RadarConfigParser().parseFile( config_file );
  assert_throw<logic_error>(&run_wrong2);
  run_paused_continued();
  run_simulator();


  return 0;
}


