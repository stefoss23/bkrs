#include <math.h>

#include <iostream>
#include <vector>

#include <radsim/utils/assert.hpp>

#include <radsim/mathematics/constants.hpp>
#include <radsim/mathematics/approx_function.hpp>
#include <radsim/mathematics/math_vector.hpp>
#include <radsim/mathematics/mathutils.hpp>

#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>
#include <radsim/radar/radar.hpp>
#include <radsim/radar/beam_pattern.hpp>

using namespace std;
using namespace radsim;

double RandReplacement(unsigned int * seed) {
  return 0.5 + 0 * (*seed);
}




void test_radar(const string& config_file) {

  auto config = RadarConfigParser().parseFile(config_file);
  config.setADCMode(ADCMode::Power);
  config.setADCResolution(16);
  config.setADCMin2Noise(20.0 / (double)65536 );
  Radar radar(config);

  assertTrue( radar.getToAddClutter() );
  assertTrue( radar.getToAddNoise() );
  assertTrue( radar.getToAddTarget() );
  assertTrue( radar.getUsePdf() );
  radar.setToAddNoise(false);
  radar.setToAddClutter(false);
  radar.setToAddTarget(false);
  radar.setUsePdf(false);
  assertFalse( radar.getToAddNoise() );
  assertFalse( radar.getToAddClutter() );
  assertFalse( radar.getToAddTarget() );
  assertFalse( radar.getUsePdf() );
  radar.setUsePdf(true);

  double range_bin = radar.getRangeBin(); //m
  double min_dist = radar.getMinimumRange(); //m
  assertComplexEqual(min_dist + 3 * range_bin, radar.getRange(3), 1e-3); 

  //Test return of a sampled pulse from noise
  //includes a single target
  radar.setToAddNoise(true);
  assertDoubleEqual(radar.getMinimumRange(), 500, 1e-2);
  assertDoubleEqual(radar.getUnAmbiguousRange(), 20000, 1e-2);
  assertDoubleEqual(radar.getRangeBin(), 37.5, 1e-3);

  auto adc = radar.getADC();
  
  double AvgNoise = radar.getAvgNoise();
  auto pulse_data = radar.generatePulseData();
  auto& registry = pulse_data.registry;
  cout << "Reg[0] = " << registry[0] << endl;
  assertTrue(  !(registry[0] == registry[1] && registry[1] == registry[2] &&
                 registry[2] == registry[3] && registry[3] == registry[4])  );

  //testing pdf-setting
  radar.setUsePdf(false);
  radar.reset();
  PulseData pulse1 = radar.generatePulseData();
  assertTrue( pulse1.isOriginal() );
  assertTrue( pulse1.hasOriginalRegistry() );
  registry = pulse1.registry;
  unsigned short Signal = adc.convertSignal(AvgNoise);
  assertDoubleEqual( registry[0], Signal, 1e-3 );
  radar.setUsePdf(true);

  radar.setRandomParameters(true, 0, &RandReplacement);
  radar.reset();
  PulseData pulse2 = radar.generatePulseData();
  assertTrue( pulse2.isOriginal() );
  assertTrue( pulse2.hasOriginalRegistry() );
  registry = pulse2.registry;
  assertIntEqual( registry[3], 2271 );
  assertIntEqual( registry[9], 2271 );
  assertIntEqual( registry.size(), 507);
  ADC A = radar.getADC();

  //test shape of filtered pulse
  DoubleApproxFunction s0 = radar.getFilteredPulse();
  assertComplexEqual( s0.output(0), 0.56, 5e-2 );
  double PulseWidth = radar.getPulseWidth();
  assertComplexEqual( s0.output(PulseWidth), 0.56, 5e-2 );
  
}


/*
The created situation is that of a radar pointing in the x direction. 
The target is at a distance correspondinig to rand bin # 253. 
The filtered pulse is overridden with a square pulse for easier calculation

                        _______
                       |       |
Pulse     _____________|       |______________
           |       |       |       |       | 
bin       251     252     253     254     255   
*/
void test_radar_equation(RadarConfig config) {

  config.setADCMode(ADCMode::Power);
  config.setADCResolution(14);
  config.setADCMin2Noise(1.0 );

  Radar radar( config );
  radar.setToAddClutter(false);
  radar.setToAddNoise(false);
  radar.setToUseFilteredPulse(false);

  int bin_index = 253; //creating a bin index on which the target is exactly located. 
  double base_distance = radar.getRange(bin_index) - 0.5 * radar.getRangeBin(); //m
  double cross_section = 50.0; //m2
  Target target({base_distance, 0, 0}, cross_section);

  double P = config.getPeakPower();
  double G = config.getAntennaeGain(); //unit
  double l = speed_of_light / config.getFrequency(); //m/s

  double received_power = P * G * G * cross_section * l * l / 
                          (  4*4*4 * pi*pi*pi * base_distance*base_distance*base_distance*base_distance ); //W

  auto adc = radar.getADC();
  int measurement = adc.convertSignal(received_power);
  auto pulse_data = radar.generatePulseData({target});
  assertDoubleEqual( pulse_data.registry[bin_index], measurement, 1.0e-2 );

  double deg_to_rad = pi / 180.0;
  auto hor_shape = radar.getHorizontalBeamShape();
  assertComplexEqual( hor_shape.output(0), 1, 1e-4 );
  assertComplexEqual( hor_shape.output(1.0 * deg_to_rad), 0.5, 1e-4 );
  assertComplexEqual( hor_shape.output(2.0 * deg_to_rad), 0.0625, 1e-4 );
  auto el_shape = radar.getElevationBeamShape();
  assertComplexEqual( el_shape.output(0), 1, 1e-4 );
  assertComplexEqual( el_shape.output(20.0 * deg_to_rad), 0.5, 1e-4 );
  assertComplexEqual( el_shape.output(40.0 * deg_to_rad), 0.0625, 1e-4 );
}


//check that the simulation clock advances PRT per pulse
//check default initial azimith = North
//check rotation
void test_naval_radar(const RadarConfig& config) {
  Radar radar(config);
  assertDoubleEqual( radar.getCurrentHorTheta(), 0.5 * pi, 1e-5 );

  assertComplexEqual( radar.getInitialHorTheta(), 0.5 * pi, 1e-4 );
  assertComplexEqual( radar.getAntRotSpeed(), -pi, 1e-4 );
  math_vector boresight = radar.getCurrentBoresight();
  assertTrue( boresight[0] < 1e-4 );
  assertComplexEqual( boresight[1], 1, 1e-4 );

  assertComplexEqual( radar.getPRT(), 2e-3, 1e-4 );
  radar.generatePulseData();
  radar.generatePulseData();
  radar.generatePulseData();
  assertComplexEqual( radar.getCurrentTime(), 6e-3, 1e-4 );
  boresight = radar.getCurrentBoresight();
  double angle = 3 * (180.0 * pi / 180.0) * 2e-3; //3 * rotation_speed * PRT
  assertDoubleEqual( radar.getCurrentHorTheta(), 0.5 * pi - angle, 1e-5 );
  assertComplexEqual( boresight[0], sin(angle), 1e-3 );
  assertComplexEqual( boresight[1], cos(angle), 1e-3 );
}



int main(int argc , char ** argv) {

  const string config_file = string(argv[1]) + "/radar_configs/short_range_radar.txt";
  RadarConfigParser parser;
  RadarConfig config = parser.parseFile(config_file);
  test_radar(config_file);
  test_radar_equation(config);

  const string config_file_nav = string(argv[1]) + "/radar_configs/naval_radar.txt";
  auto config_nav = parser.parseFile(config_file_nav);
  test_naval_radar(config_nav);

  return 0;
}
