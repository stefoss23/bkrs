#include <assert.h>
#include <math.h>

#include <iostream>
#include <vector>

#include <radsim/utils/utils.hpp>
#include <radsim/utils/assert.hpp>

#include <radsim/mathematics/constants.hpp>
#include <radsim/mathematics/approx_function.hpp>
#include <radsim/mathematics/mathutils.hpp>

#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>
#include <radsim/radar/radar.hpp>
#include <radsim/radar/beam_pattern.hpp>

using namespace std;
using namespace radsim;


double RandReplacement(unsigned int * seed) {
  return 0.5;
}


/*
Test that an unfiltered pulse has correct shape and time posiition

                                  _________________
                                 |                 |
                                 |                 |
                                 |                 |
Pulse ---------------------------|                 |------------------
                                     
                           <-----><---------------->
                            bin      pulsewidth
                           offset
                                    range bin
                                    <------->
time/range ----------------------------------------------------------------->
Bin index         |        |        |        |        |
                 252      253      254      255      256
*/
void test_unfiltered_signal(RadarConfig config) {
  config.setSamplingTime( 0.5 * config.getPulseWidth() * 1e6 );
  Radar radar( config );
  radar.setToAddClutter(false);
  radar.setUsePdf(false);
  radar.setToAddNoise(false);
  radar.setToUseFilteredPulse(false);
  radar.setToUseFilteredPulse(true);
  radar.setToUseFilteredPulse(false);
  double range_bin = radar.getRangeBin(); //m
  double avg_noise = radar.getAvgNoise(); //W

  double signal_override = 40 * avg_noise; //W
  double filtered_signal = radar.getFilteredPulse().output(0) * powerToAmp(signal_override); //amp
  unsigned short digital_signal = radar.getADC().convertSignal(filtered_signal);

  int bin_index = 253; //creating a bin index on which the target is exactly located. 
  double base_distance = radar.getRange(bin_index); //m
  TargetCollection targets;
  targets.emplace_back(  (math_vector){base_distance, 0, 0}, 1.0  );
  Target& target = targets.front();

  target.setPosition({base_distance + 0.01 * range_bin, 0, 0});
  auto pulse_data = radar.generatePulseData(targets, true, signal_override);
  auto& registry = pulse_data.registry;
  assertIntEqual( registry[253], 0);

  target.setPosition({base_distance - 0.01 * range_bin, 0, 0});
  pulse_data = radar.generatePulseData(targets, true, signal_override);
  registry = pulse_data.registry;
  assertIntEqual( registry[253], digital_signal);

  //Testing for bool variable to_add_target
  radar.setToAddTarget(false);
  pulse_data = radar.generatePulseData(targets, true, signal_override);
  registry = pulse_data.registry;
  assertIntEqual( registry[253], 0);  
  radar.setToAddTarget(true);

  target.setPosition({base_distance - 0.5 * range_bin, 0, 0});
  pulse_data = radar.generatePulseData(targets, true, signal_override);
  registry = pulse_data.registry;
  assertIntEqual( registry[253], digital_signal);

  target.setPosition({base_distance - 0.99 * range_bin, 0, 0});
  pulse_data = radar.generatePulseData(targets, true, signal_override);
  registry = pulse_data.registry;
  assertIntEqual( registry[253], digital_signal);

  target.setPosition({base_distance - 1.01 * range_bin, 0, 0});
  pulse_data = radar.generatePulseData(targets, true, signal_override);
  registry = pulse_data.registry;
  assertIntEqual( registry[253], digital_signal);

  target.setPosition({base_distance - 1.5 * range_bin, 0, 0});
  pulse_data = radar.generatePulseData(targets, true, signal_override);
  registry = pulse_data.registry;
  assertIntEqual( registry[253], digital_signal);

  target.setPosition({base_distance - 1.99 * range_bin, 0, 0});
  pulse_data = radar.generatePulseData(targets, true, signal_override);
  registry = pulse_data.registry;
  assertIntEqual( registry[253], digital_signal);

  target.setPosition({base_distance - 2.01 * range_bin, 0, 0});
  pulse_data = radar.generatePulseData(targets, true, signal_override);
  registry = pulse_data.registry;
  assertIntEqual( registry[253], 0);
}

/*test that signal from target is correct aftering filtering and digital conversion
Please refer to TechnicalDocument: Signal Strength at Sampling Stage:
                                           _______
                                          /       \
                                         /         \
                                        /           \
Filtered Pulse Amplitude --------------/             \----------------------
timeline ---------------------------------------------------------------------->
                               |         |         |          |
bin index                     252       253       254        255 

                                         <--------->
                                          pulsewidth

Radar propgation is ignored is the signal is set to override more. 
Basic Pulse -> Bandpass Filtering -> Analog to digital conversion -> integer output
*/
void test_basic_signal(const RadarConfig& config) {
  Radar radar( config );
  double avg_noise = radar.getAvgNoise(); //W
  radar.setToAddClutter(false);
  radar.setUsePdf(false);
  radar.setToAddNoise(false);

  double signal_override = 40 * avg_noise; //W
  double signal_amplitude = radar.getFilteredPulse().output(0) * powerToAmp(signal_override); //amp
  double signal_power = signal_amplitude * signal_amplitude; //W
  unsigned short digital_signal = radar.getADC().convertSignal(signal_power);

  int bin_index = 253; //creating a bin index on which the target is exactly located. 
  double base_distance = radar.getRange(bin_index); //m
  TargetCollection targets;
  targets.emplace_back( (math_vector){base_distance, 0, 0}, 1.0 );

  //Test correct posision with regards to time
  auto pulse_data = radar.generatePulseData(targets, true, signal_override);
  const auto& registry = pulse_data.registry;
  //check some signal at 253 and 254, nothing "elsewhere"
  assertIntEqual(registry[253], digital_signal);
  assertIntEqual(registry[254], digital_signal);
  assertIntEqual(registry[200], 0);
  assertIntEqual(registry[400], 0);
}


//Test signal from target beyond unambiguous range
/*
                                                                                     Target
                                                                                        |
                                                                                        |
               base dist.                                                      base d.  V
               <-------->                                                      <-------->                        
distance       ------------------------------------------------------------------------------>
               <------------------------------><------------------------------>
                 Unambiguous Rng                 Unambiguous Rng

               Start emit                      Start emit                     Start emit
               Pulse 0                         Pulse1                         Pulse 2
               |                               |                              |
               V                               V                              V
time           ------------------------------------------------------------------------------>
               <------------------------------><------------------------------>
                 Pulse repetition time           Pulse repetition time 
*/
void test_ambiguous_range(RadarConfig config) {
  config.setMaximumReceiveTime(0.125);
  Radar radar( config );
  double maxrange = radar.getUnAmbiguousRange(); //m
  double avg_noise = radar.getAvgNoise(); //W
  radar.setToAddClutter(false);
  radar.setUsePdf(false);
  radar.setToAddNoise(false);

  int bin_index = 253; //creating a bin index on which the target is exactly located. 
  double base_distance = radar.getRange(bin_index); //m
  double target_distance = 2 * maxrange + base_distance;
  TargetCollection targets;
  targets.emplace_back( (math_vector){target_distance, 0, 0}, 0.0 );
  assertIntEqual( radar.getCurrentCarrySize(), 0 );

  PulseData pulse0 = radar.generatePulseData(targets, true, 40 * avg_noise);
  assertIntEqual( radar.getCurrentCarrySize(), 1 );
  const auto& packet0 = pulse0.registry;
  //The target is at such a far distance that it cannot appear in pulse 0.
  assertIntEqual( packet0[200], 0 );
  assertIntEqual( packet0[253], 0 );
  assertIntEqual( packet0[254], 0 );
  assertIntEqual( packet0[400], 0 );
  //And neither in pulse 1 
  PulseData pulse1 = radar.generatePulseData(targets, true, 40 * avg_noise);
  assertIntEqual( radar.getCurrentCarrySize(), 2 );
  const auto& packet1 = pulse1.registry;
  assertIntEqual( packet1[200], 0 );
  assertIntEqual( packet1[253], 0 );
  assertIntEqual( packet1[254], 0 );
  assertIntEqual( packet1[400], 0 );
  //But instead in Pulse 2
  PulseData pulse2 = radar.generatePulseData(targets, true, 40 * avg_noise);
  assertIntEqual( radar.getCurrentCarrySize(), 2 );
  const auto& packet2 = pulse2.registry;
  assertIntEqual( packet2[200], 0 );
  assertIntEqual( packet2[253], 658 );
  assertIntEqual( packet2[254], 658 );
  assertIntEqual( packet2[400], 0 );
  //And also in Pulse 3
  PulseData pulse3 = radar.generatePulseData(targets, true, 40 * avg_noise);
  assertIntEqual( radar.getCurrentCarrySize(), 2 );
  assertTrue( pulse3.isOriginal() );
  assertTrue( pulse3.hasOriginalRegistry() );
  const auto& packet3 = pulse3.registry;
  assertIntEqual( packet3[200], 0 );
  assertTrue( packet3[253] > 50 );
  assertTrue( packet3[254] > 50 );
  assertIntEqual( packet3[400], 0 );

  radar.generatePulseData();
  assertIntEqual( radar.getCurrentCarrySize(), 1 );
  radar.generatePulseData();
  assertIntEqual( radar.getCurrentCarrySize(), 0 );

}


//This tests target signal at various situations where target is offset horizontally from boresight. 
void test_angle_offset(RadarConfig config) {

  config.setADCMode(ADCMode::Power);
  config.setADCResolution(14);
  config.setADCMin2Noise(0.005 );  
  config.setHorizontalBeamShape(BeamPattern::Triangular);
  config.setElevationBeamShape(BeamPattern::Triangular);
  Radar radar( config );
  double maxrange = radar.getUnAmbiguousRange();
  double avg_noise = radar.getAvgNoise(); //W
  radar.setToAddClutter(false);
  radar.setUsePdf(false);
  radar.setToAddNoise(false);
  radar.setToUseFilteredPulse(false);

  double deg_to_rad = pi / 180.0;
  double pw = radar.getPulseWidth();
  unsigned short max_signal = radar.getADC().getNumLevels() - 1;

  int bin_index = 253; //creating a bin index on which the target is exactly located. 
  double base_distance = radar.getRange(bin_index) - 0.5 * radar.getRangeBin(); //m
  double signal_override = 40 * avg_noise; //W

  /*
  ------ Both target and radar are stationary, moving target in an arc crossing the east axis. 
  Bandpass filtering has been removed to created an incomung unmodulated pulse.
  If in boresight & at base_distance, full intensity Is acheived. 
  horizontal beamwidth is 2 degrees, so a 1 degree variation should halve the beam power at target
  i.e. reduce received power to 1/4.
  beam shape is triangular w /equal sides. 
  ADC resolution is set to 10000.
  Sensitivity is equal to 0.005 * avg_noise 
  Received at boresight is 40 * avg_noise
  Digital_signal = Resolution * 40 * avg_noise / 0.005 Noise = 8000
  */

  //at boresight
  TargetCollection targets;
  targets.emplace_back( (math_vector){base_distance, 0, 0}, 0.0 );
  radar.reset();
  auto pulse_data = radar.generatePulseData(targets, true , signal_override);
  auto& reg = pulse_data.registry;
  assert( double_equal(reg[253], 8000, 1e-2) );

  //1 degree to the left, measure should be 2000
  targets.front().setPosition({base_distance * cos(1.0 * deg_to_rad) , base_distance * sin(1.0 * deg_to_rad), 0});
  radar.reset();
  pulse_data = radar.generatePulseData(targets, true , signal_override);
  reg = pulse_data.registry;
  assert( double_equal(reg[253], 2000, 1e-2) );

  //2 degrees to the left, measure should be 0
  targets.front().setPosition({base_distance * cos(2.0 * deg_to_rad) , base_distance * sin(2.0 * deg_to_rad), 0});
  radar.reset();
  pulse_data = radar.generatePulseData(targets, true , signal_override);
  reg = pulse_data.registry;
  assert( double_equal(reg[253], 0, 1e-2) );

  //1 degree to the right, measure should be 2000
  targets.front().setPosition({base_distance * cos(-1.0 * deg_to_rad) , base_distance * sin(-1.0 * deg_to_rad), 0});
  radar.reset();
  pulse_data = radar.generatePulseData(targets, true , signal_override);
  reg = pulse_data.registry;
  assert( double_equal(reg[253], 2000, 1e-2) );

  //2 degrees to the right, measure should be 0
  targets.front().setPosition({base_distance * cos(-2.0 * deg_to_rad) , base_distance * sin(-2.0 * deg_to_rad), 0});
  radar.reset();
  pulse_data = radar.generatePulseData(targets, true , signal_override);
  reg = pulse_data.registry;
  assert( double_equal(reg[253], 0, 1e-2) );


  //------ Both target and radar are stationary, moving radar in an arc crossing the east axis. 
  targets.front().setPosition({base_distance, 0, 0});

  radar.setInitialHorTheta(1.0 * deg_to_rad); //1 degree north
  radar.reset();
  pulse_data = radar.generatePulseData(targets, true , signal_override);
  reg = pulse_data.registry;
  assert( double_equal(reg[253], 2000, 1e-2) );

  radar.setInitialHorTheta(- 1.0 * deg_to_rad); //1 degree south
  radar.reset();
  pulse_data = radar.generatePulseData(targets, true , signal_override);
  reg = pulse_data.registry;
  assert( double_equal(reg[253], 2000, 1e-2) );

  /*----- testing with moveable target, Radar antannae is stationary
    Target start at -2 deg offset, moving 1 deg per pulse.*/
  radar.setInitialHorTheta(0);
  double T = radar.getPRT(); //pulse repetition time
  //creating a target path
  vector<double> time_entry = {0.0*T, 1.0*T, 2.0*T, 3.0*T, 4.0*T};
  vector<math_vector> pos_entry = {   math_vector{ base_distance * cos(- 2.0 * deg_to_rad), base_distance * sin( -2.0 * deg_to_rad ), 0 },
                                      math_vector{ base_distance * cos(- 1.0 * deg_to_rad), base_distance * sin( -1.0 * deg_to_rad ), 0 },
                                      math_vector{ base_distance * cos(  0.0 * deg_to_rad), base_distance * sin(  0.0 * deg_to_rad ), 0 },
                                      math_vector{ base_distance * cos(  1.0 * deg_to_rad), base_distance * sin(  1.0 * deg_to_rad ), 0 },
                                      math_vector{ base_distance * cos(  2.0 * deg_to_rad), base_distance * sin(  2.0 * deg_to_rad ), 0 } };
  VectorApproxFunction path(time_entry, pos_entry);
  
  auto p = path.output(0);
  targets.clear();
  targets.emplace_back( move(path), 0.0 );
  
  radar.reset();
  auto pulse_data0 = radar.generatePulseData(targets, true, signal_override);
  auto pulse_data1 = radar.generatePulseData(targets, true, signal_override);
  auto pulse_data2 = radar.generatePulseData(targets, true, signal_override);
  auto pulse_data3 = radar.generatePulseData(targets, true, signal_override);
  auto pulse_data4 = radar.generatePulseData(targets, true, signal_override);
  assert( pulse_data0.getStartTime() == 0.0 );
  assert( double_equal(pulse_data1.getStartTime(), T, 1e-3 ));
  assert( double_equal(pulse_data0.registry[253], 0, 1e-2 )  );    //-2 deg offser
  assert( double_equal(pulse_data1.registry[253], 2000, 1e-2 )  ); //-1 deg offset                         
  assert( double_equal(pulse_data2.registry[253], 8000, 1e-2 )  ); //0 deg = boresight
  assert( double_equal(pulse_data3.registry[253], 2000, 1e-2 )  ); //+1 deg offset
  assert( double_equal(pulse_data4.registry[253], 0000, 1e-2 )  ); //+1 deg offset

  /*----- testing with moveable target, Radar antannae is stationary
    ----- target has some height, the same azimuth deviation applies
    Target start at -2 deg offset, moving 1 deg per pulse.*/
  radar.setInitialHorTheta(0);
  T = radar.getPRT(); //pulse repetition time
  //creating a target path
  time_entry = {0.0*T, 1.0*T, 2.0*T, 3.0*T, 4.0*T};
                      pos_entry = {   math_vector_spherical( base_distance, 20.0 * deg_to_rad, -2.0 * deg_to_rad),
                                      math_vector_spherical( base_distance, 20.0 * deg_to_rad, -1.0 * deg_to_rad),
                                      math_vector_spherical( base_distance, 20.0 * deg_to_rad,  0.0),
                                      math_vector_spherical( base_distance, 20.0 * deg_to_rad,  1.0 * deg_to_rad),
                                      math_vector_spherical( base_distance, 20.0 * deg_to_rad,  2.0 * deg_to_rad) };

  path = VectorApproxFunction(time_entry, pos_entry);
  targets.clear();
  targets.emplace_back( move(path), 0.0 );
  
  radar.reset();
  pulse_data0 = radar.generatePulseData(targets, true, signal_override);
  pulse_data1 = radar.generatePulseData(targets, true, signal_override);
  pulse_data2 = radar.generatePulseData(targets, true, signal_override);
  pulse_data3 = radar.generatePulseData(targets, true, signal_override);
  pulse_data4 = radar.generatePulseData(targets, true, signal_override);
  assert( pulse_data0.getStartTime() == 0.0 );
  assert( double_equal(pulse_data1.getStartTime(), T, 1e-3 ));
  assert( double_equal(pulse_data0.registry[253],    0, 1e-2 )  );    //-2 deg az offser, -20 deg el offset
  assert( double_equal(pulse_data1.registry[253],  500, 1e-2 )  ); //-1 deg az offset, -20 deg el offset                         
  assert( double_equal(pulse_data2.registry[253], 2000, 1e-2 )  ); //0 deg = az boresight, -20 deg el offset
  assert( double_equal(pulse_data3.registry[253],  500, 1e-2 )  ); //+1 deg az offset, - 20 deg el offset
  assert( double_equal(pulse_data4.registry[253],    0, 1e-2 )  ); //+1 deg az offset, -20 deg el offset*/

  /*----- testing with moveable target, Radar antannae is stationary
    ----- Elevation movement
    Target start at -2 deg offset, moving 1 deg per pulse.*/
  radar.setInitialHorTheta(0);
  T = radar.getPRT(); //pulse repetition time
  //creating a target path
  time_entry = {0.0*T, 1.0*T, 2.0*T, 3.0*T, 4.0*T};
                      pos_entry = {   math_vector_spherical( base_distance, 40.0 * deg_to_rad,  0.0),
                                      math_vector_spherical( base_distance, 20.0 * deg_to_rad,  0.0),
                                      math_vector_spherical( base_distance,  0.0 * deg_to_rad,  0.0),
                                      math_vector_spherical( base_distance, 20.0 * deg_to_rad,  0.0),
                                      math_vector_spherical( base_distance, 40.0 * deg_to_rad,  0.0) };

  path = VectorApproxFunction(time_entry, pos_entry);
  
  targets.clear();
  targets.emplace_back( move(path), 0.0 );
  
  radar.reset();
  pulse_data0 = radar.generatePulseData(targets, true, signal_override);
  pulse_data1 = radar.generatePulseData(targets, true, signal_override);
  pulse_data2 = radar.generatePulseData(targets, true, signal_override);
  pulse_data3 = radar.generatePulseData(targets, true, signal_override);
  pulse_data4 = radar.generatePulseData(targets, true, signal_override);
  assert( pulse_data0.getStartTime() == 0.0 );
  assert( double_equal(pulse_data1.getStartTime(), T, 1e-3 ));
  assert( double_equal(pulse_data0.registry[253],    0, 1e-2 )  );    //-40 deg el offset
  assert( double_equal(pulse_data1.registry[253], 2000, 1e-2 )  ); //-20 deg el offset                         
  assert( double_equal(pulse_data2.registry[253], 8000, 1e-2 )  ); //0 deg = boresight
  assert( double_equal(pulse_data3.registry[253], 2000, 1e-2 )  ); //+20 deg el offset
  assert( double_equal(pulse_data4.registry[253],    0, 1e-2 )  ); //+40 deg el offset

  /*----- testing with rotating antennae, target is stationary
    Antennae moves by 1 deg per pulse period. 
  */
  radar.setInitialHorTheta(- 2.0 * deg_to_rad);
  radar.setAntRotSpeed( 1.0 * deg_to_rad / T ); 

  targets.front().setPosition({base_distance, 0, 0});

  radar.reset();
  pulse_data0 = radar.generatePulseData(targets, true, signal_override);
  pulse_data1 = radar.generatePulseData(targets, true, signal_override);
  pulse_data2 = radar.generatePulseData(targets, true, signal_override);
  pulse_data3 = radar.generatePulseData(targets, true, signal_override);
  pulse_data4 = radar.generatePulseData(targets, true, signal_override);
  assert( pulse_data2.getBoresight()[1] < 1e-5 ); //pulse was emitting in x -direction
  assert( double_equal(pulse_data0.registry[253], 0, 1e-2 )  );
  assert( double_equal(pulse_data1.registry[253], 2000, 1e-2 )  );                            
  assert( double_equal(pulse_data2.registry[253], 8000, 1e-2 )  );
  assert( double_equal(pulse_data3.registry[253], 2000, 1e-2 )  );
  assert( double_equal(pulse_data4.registry[253], 0, 1e-2 )  );

  //------ testing const target, w/ rotatable antennae, target beoynd unambiguous range
  //------ stationary target
  targets.front().setPosition({maxrange + base_distance , 0, 0});
  radar.reset();
  pulse_data0 = radar.generatePulseData(targets, true, signal_override);
  pulse_data1 = radar.generatePulseData(targets, true, signal_override);
  pulse_data2 = radar.generatePulseData(targets, true, signal_override);
  pulse_data3 = radar.generatePulseData(targets, true, signal_override);
  pulse_data4 = radar.generatePulseData(targets, true, signal_override);
  assert( double_equal(pulse_data0.registry[253], 0, 1e-2 )  );     //0, because target beyond unamb. range
  assert( double_equal(pulse_data1.registry[253], 0, 1e-2 )  );     //0, because of offset in pulse 0, emission at -2 deg 0 => 
                                                                    //no intensity
  assert( double_equal(pulse_data2.registry[253], 4000, 1e-2 )  );  //received at boresight, emitted at deg -1 with half intensity
  assert( double_equal(pulse_data3.registry[253], 4000, 1e-2 )  );  //received with half intensity at +1 deg, emitted at boresight 
                                                                    //with full intensity
  assert( double_equal(pulse_data4.registry[253], 0, 1e-2 )  );     //received at +2 deg at zero intensity. 
}


//testing antiphase, e.g. noise and target are 180 deg out of phase.
//target is set to be of similar strength as noise.
//Total amplitude = noise + exp(2 * pi * i * phase) * target_signal
//if phase == 0.5 => an antiphase situation occur.
void test_phase_and_multiple_targets(RadarConfig config) {
  config.setADCMode(ADCMode::Power);
  config.setADCResolution(14);
  config.setADCMin2Noise(0.005 );  
  Radar radar( config );
  radar.setToUseFilteredPulse(false);
  radar.setRandomParameters(true, 0, &RandReplacement); //when signals are added torgether is rand return 0.5. This is antiphase. 
                                                        //this overwrites the internal Radar RNG function.
  radar.setUsePdf(false);

  double avg_noise = radar.getAvgNoise(); //W
  double sensitivity = 0.005 * avg_noise;
  double max_range = radar.getUnAmbiguousRange();
  auto digital_signal = radar.getADC().convertSignal( avg_noise );

  TargetCollection targets;

  int bin1 = 253; //creating a bin index on which the target is exactly located. 
  double distance1 = radar.getRange(bin1) - 0.5 * radar.getRangeBin(); //m
  targets.emplace_back( (math_vector){distance1, 0, 0}, 1.0 );
  targets.emplace_back( (math_vector){distance1 + 1.0 * max_range, 0, 0}, 1.0 );

  int bin2 = 302;
  double distance2 = radar.getRange(bin2) - 0.5 * radar.getRangeBin(); //m
  targets.emplace_back( (math_vector){distance2 + 1.0 * max_range, 0, 0}, 1.0 );   
  targets.emplace_back( (math_vector){distance2 + 2.0 * max_range, 0, 0}, 1.0 );

  int bin3 = 105;
  double distance3 = radar.getRange(bin3) - 0.5 * radar.getRangeBin(); //m
  targets.emplace_back( (math_vector){distance3 + 2.0 * max_range, 0, 0}, 1.0 );

  radar.reset();
  radar.setToAddNoise(false);
  auto pulse = radar.generatePulseData(targets, true , avg_noise);
  auto& reg = pulse.registry;
  assert( double_equal(reg[bin1], digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin2], 0, 1e-2 )  );
  assert( double_equal(reg[bin3], 0, 1e-2 )  );
  pulse = radar.generatePulseData(targets, true , avg_noise);
  reg = pulse.registry;
  assert( double_equal(reg[bin1], 4 * digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin2], digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin3], 0, 1e-2 )  );  
  pulse = radar.generatePulseData(targets, true , avg_noise);
  reg = pulse.registry;
  assert( double_equal(reg[bin1], 4 * digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin2], 4 * digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin3], digital_signal, 1e-2 )  );

  radar.setToAddNoise(true);
  radar.reset();
  pulse = radar.generatePulseData();
  reg = pulse.registry;
  assert( double_equal(reg[bin1], digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin2], digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin3], digital_signal, 1e-2 )  );

  radar.reset();
  pulse = radar.generatePulseData(targets, true , avg_noise);
  reg = pulse.registry;
  assert( double_equal(reg[bin1], 0, 1e-2 )  );
  assert( double_equal(reg[bin2], digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin3], digital_signal, 1e-2 )  );
  pulse = radar.generatePulseData(targets, true , avg_noise);
  reg = pulse.registry;
  assert( double_equal(reg[bin1], digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin2], 0, 1e-2 )  );
  assert( double_equal(reg[bin3], digital_signal, 1e-2 )  );  
  pulse = radar.generatePulseData(targets, true , avg_noise);
  reg = pulse.registry;
  assert( double_equal(reg[bin1], digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin2], digital_signal, 1e-2 )  );
  assert( double_equal(reg[bin3], 0, 1e-2 )  );
}


int main(int argc , char ** argv) {

  const string config_file = string(argv[1]) + "/radar_configs/short_range_radar.txt";
  RadarConfigParser parser;
  RadarConfig config = parser.parseFile(config_file);
  test_unfiltered_signal(config);
  test_basic_signal(config);
  test_ambiguous_range(config);
  test_angle_offset(config);
  test_phase_and_multiple_targets(config);
}
