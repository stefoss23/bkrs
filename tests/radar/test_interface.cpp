/* This is the first test of real time processing with rads. */

#include <vector>
#include <memory>
#include <exception>

#include <radsim/utils/test_utils.hpp>
#include <radsim/utils/assert.hpp>
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

  assertComplexEqual(com.getRange(200), radar.getRange(200), 1e-4);
  assertComplexEqual(com.getRange(400), radar.getRange(400), 1e-4);

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
      assertTrue( data.getStartTime() > test_data_time );
      test_data_time = data.getStartTime();  //s    

      vector<unsigned short> registry = move(data.registry);

      assertIntEqual( registry[400], 0 );
      assertIntEqual( registry[200], 0 );
      if (pulse_indx > 0) {
        assertTrue( registry[253] > 100 );
        assertTrue( registry[254] > 100 );
      }
      else {
        assertIntEqual( registry[253], 0 );  //the target is begyond unambiguous range in pulse 0
        assertIntEqual( registry[254], 0 );
      }

      assertTrue( data.getBoresight() == boresight );
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
  RadarInterface com(config, {}, 0.02);
  double delta_time = 0.25;
  double max_time = 0.25;

  // --- First interval -----------------------
  com.start();
  while (com.getSimTime() < delta_time) {
  }
  com.stop();
  assertTrue( com.dataReady() );
  auto data = com.getData();
  while (com.dataReady()) data = com.getData(); //getting the last data package of the queue

  assertFalse (com.dataReady());
  assertDoubleEqual( com.getSimTime(), max_time, 1.0e-1 );

  int old_time = max_time;
  max_time += delta_time;

  // ---- Second Interval ----------------------
  com.start();
  while (com.getSimTime() < max_time) {
    assertTrue(com.getSimTime() >= old_time);
  }
  com.stop();
  assertDoubleEqual( com.getSimTime(), max_time, 1.0e-1 );

  com.dataReady();
  auto data1 = com.getData(); // getting the first data package of the queue
  double prt = data1.getStartTime() - data.getStartTime(); //comparing the last package of the first interval and the 
                                                           //first package of the 2nd interval. Time difference should be PRT
  assertDoubleEqual( prt, config.getPRT(), 1e-4 );
  
  max_time += delta_time;

  // --- Third Interval -------------------------
  com.start();
  while (com.getSimTime() < max_time) {   
    if (com.dataReady()) com.getData();
  }
  com.stop();
  assertDoubleEqual( com.getSimTime(), max_time, 1.0e-1 );
  
  max_time += delta_time;
  Timer timer;

  // --- Fourth Interval -------------------------
  com.start();
  while (com.getSimTime() < max_time) {
    if (com.dataReady()) com.getData();
  }
  com.stop();
  double elapsed_time = timer.elapsed();
  assertDoubleEqual( elapsed_time, delta_time, 2.0e-1 );
}


void run_reset() {
  RadarInterface com(config, {}, 0.02);
  double max_time = 0.25;

  com.start();
  while (com.getSimTime() < max_time) {
  }
  com.stop();

  assertTrue( com.getSimTime() > 0);
  assertTrue( com.dataReady() );
  
  com.reset();
  assertTrue( com.getSimTime() == 0);
  assertFalse( com.dataReady() );

  com.start();
  while (com.getSimTime() < max_time) {
  }
  com.stop();

  assertTrue( com.getSimTime() > 0);
  assertTrue( com.dataReady() );

  com.reset(3.0);
  assertTrue( com.getSimTime() == 3.0);
  assertFalse( com.dataReady() );

  max_time = 3.25;

  com.start();
  while (com.getSimTime() < max_time) {
  }
  com.stop();

  assertTrue( com.getSimTime() > max_time);
}


void run_wrong2() {
  RadarInterface com(config, {});
  com.start();
  com.start();
}


void run_wrong3() {
  RadarInterface com(config, {});
  com.start();
  com.reset();
}



int main(int argc , char ** argv) {

  const string config_file = string(argv[1]) + "/radar_configs/short_range_radar.txt";
  config = RadarConfigParser().parseFile( config_file );

  assertThrow(run_wrong2(), logic_error);
  assertThrow(run_wrong3(), logic_error);

  run_paused_continued();
  run_reset();
  run_simulator();


  return 0;
}


