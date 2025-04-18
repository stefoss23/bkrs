#include <math.h>
#include <iostream>

#include <radsim/utils/assert.hpp>

#include <radsim/radar/adc.hpp>

using namespace std;
using namespace radsim;

void test_adc_effect_mode() {
  double min_level  = 1e-14; //W
  int    resolution = 10; //bits
  ADC adc(resolution, ADCMode::Power, min_level); 
  int num_levels = adc.getNumLevels();
  assertIntEqual( num_levels, 1024 );
  assertComplexEqual( adc.getSensitivity(), min_level, 1e-2 );
  assertIntEqual( adc.convertSignal(min_level * 1024) , num_levels - 1);
  assertIntEqual(adc.convertSignal(min_level * 122.001),  122);
}

void test_adc_log_mode() {
  double max_level  = 1e-12; //W
  double min_level  = 1e-14; //W
  int    resolution = 9; //bits
  ADC adc(resolution, ADCMode::Logarithm, min_level, max_level); 
  assertIntEqual( adc.convertSignal(max_level * 1.01) , 511 );
  assertIntEqual( adc.convertSignal(min_level * 0.5) , 0 );
  assertIntEqual( adc.convertSignal(min_level * 5) , 177 );
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
  assertThrow( wrong_1() , invalid_argument );
  assertThrow( wrong_2() , invalid_argument );
  assertThrow( wrong_3() , invalid_argument );
}

int main() {
  assertTrue(false);
  test_adc_effect_mode();
  test_adc_log_mode();
  test_wrong_inputs();
   
  return 0;
}

