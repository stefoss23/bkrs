#include <assert.h>
#include <math.h>
#include <iostream>

#include <radsim/utils/utils.hpp>
#include <radsim/utils/test_utils.hpp>

#include <radsim/radar/adc.hpp>

using namespace std;
using namespace radsim;

void test_adc_effect_mode() {
  double min_level  = 1e-14; //W
  int    resolution = 10; //bits
  ADC adc(resolution, ADCMode::Power, min_level); 
  int num_levels = adc.getNumLevels();
  assert( num_levels == 1024 );
  assert( complex_equal(adc.getSensitivity(), min_level, 1e-2) );
  double amplitude = sqrt(min_level * 1024); //amp
  assert(adc.convertSignal(amplitude) == num_levels - 1);
  amplitude = sqrt(min_level * 122.001); //amp
  assert(adc.convertSignal(amplitude) == 122);
}

void test_adc_log_mode() {
  double max_level  = 1e-12; //W
  double min_level  = 1e-14; //W
  int    resolution = 9; //bits
  ADC adc(resolution, ADCMode::Logarithm, min_level, max_level); 
  double amplitude = sqrt(max_level * 1.01);
  assert( adc.convertSignal(amplitude) == 511 );
  amplitude = sqrt(min_level * 0.5);
  assert( adc.convertSignal(amplitude) == 0 );
  amplitude = sqrt(min_level * 5);
  assert( adc.convertSignal(amplitude) == 177 );
}

void wrong_1() {
  ADC(10, ADCMode::Logarithm, 12.0);
}

void wrong_2() {
  ADC(10, ADCMode::Logarithm, 12.0, -5.0);
}

void wrong_3() {
  ADC(10, ADCMode::Logarithm, 20.0, 12.0);
}

void test_wrong_inputs() {
  assert_throw<std::invalid_argument>( &wrong_1 );
  assert_throw<std::invalid_argument>( &wrong_2 );
  assert_throw<std::invalid_argument>( &wrong_3 );
}

int main() {
  test_adc_effect_mode();
  test_adc_log_mode();
  test_wrong_inputs();
   
  return 0;
}

