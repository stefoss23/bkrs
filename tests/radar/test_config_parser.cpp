#include <iostream>

#include <radsim/utils/assert.hpp>

#include <radsim/mathematics/constants.hpp>

#include <radsim/radar/radar_config_parser.hpp>
#include <radsim/radar/radar_config.hpp>

using namespace std;
using namespace radsim;

void test_parse_string() {
  const string config_str = string { R"(
  Frequency        10.0 #Ghz
  PeakPower        10000.0 #W
  PulseWidth       0.25 #microsec
  PRT              2.0 #ms
  BandWidth        4.0 #Mhz
  NoiseFigure      4.0 #db
  DuplexSwitchTime 500.0 #microsec
  AntennaeGain     30.0 #db
  AzBeamWidth      2.0 #deg
  ElBeamWidth      20.0 #deg

  ADCResolution    1024 #integer
  ADCMode          Power
  ADCMin2Noise     0.01953125 #unit
  )"};
  RadarConfigParser parser;
  auto config = parser.parseString(config_str);
  config.assertParametersSet();
  assertComplexEqual( config.getSamplingTime(),
                        0.5 * config.getPulseWidth(),
                        1e-4 );
  assertComplexEqual( config.getMaximumReceiveTime(), 1.5e-3, 1e-4 );
  assertComplexEqual( config.getTheta(), 0.5 * pi, 1e-4 );
}

void test_parse_optional_params() {
  const string config_str = string { R"(
  #required params:
  Frequency        10.0 #Ghz
  PeakPower        10000.0 #W
  PulseWidth       0.25 #microsec
  PRT              2.0 #ms
  BandWidth        4.0 #Mhz
  NoiseFigure      4.0 #db
  DuplexSwitchTime 3.082222 #microsec
  AntennaeGain     30.0 #db
  AzBeamWidth      2.0 #deg
  ElBeamWidth      20.0 #deg

  ADCResolution    1024 #integer
  ADCMode          Power
  ADCMin2Noise     0.01953125 #unit

  #optional parameters:
  Azimuth          87.0 #deg
  SamplingTime     0.2 #microsec
  MaxReceiveTime   1.2 #ms
  AntRotationSpeed 2.0 #deg/s  
  )"};
  RadarConfigParser parser;
  auto config = parser.parseString(config_str);
  config.assertParametersSet();
  assertComplexEqual( config.getTheta() * 180.0 / pi, 3.0, 1e-4 );
  assertComplexEqual( config.getSamplingTime(), 0.2e-6, 1e-4 );
  assertComplexEqual( config.getMaximumReceiveTime(), 1.2e-3, 1e-4 );
  assertComplexEqual( -config.getAntRotSpeed() * 180.0 / pi, 2.0, 1e-4 );
  
}

void test_parse_double_as_integer() {
  const string config_str = string { R"(
  Frequency        10.0 #Ghz

  ADCResolution    12.2 #integer
  ADCMode          Power
  ADCMax2Noise     20.0 #unit
  )"};
  RadarConfigParser parser;
  auto config = parser.parseString(config_str);
}

//note DuplexSwitchTime is missing
void test_parse_missing_keyword() {
  const string config_str = string { R"(
  Frequency        10.0 #Ghz
  PeakPower        10000.0 #W
  PulseWidth       0.25 #microsec
  MaxReceiveTime   1333.33333 #ms
  BandWidth        4.0 #Mhz
  NoiseFigure      4.0 #db
  AntannaeGain     30.0 #db
  AzBeamWidth      2.0 #deg
  ElBeamWidth      12.0 #deg

  ADCResolution    1024 #integer
  ADCMode          Power
  ADCMax2Noise     20.0 #unit
  )"};
  RadarConfigParser parser;
  auto config = parser.parseString(config_str);
  config.assertParametersSet();
}

//note: Bandwidth not set
void test_parse_wrong_keyword_structure_a() {
  const string config_str = string { R"(
  Frequency        10.0 #Ghz
  MaxReceiveTime   1333.33333 #ms
  BandWidth
  NoiseFigure      4.0 #db
  )"};
  RadarConfigParser parser;
  RadarConfig config = parser.parseString(config_str);
}

//note: DuplexSwitchTime has two values
void test_parse_wrong_keyword_structure_b()  {
  const string config_str = string { R"(
  Frequency        10.0 #Ghz
  DuplexSwitchTime 3.082222 343.2 #microsec
  AntannaeGain     30.0 #db
  )"};
  RadarConfigParser parser;
  RadarConfig config = parser.parseString(config_str);
}

//note Frequency as string
void test_parse_wrong_keyword_structure_c(){
  const string config_str = string { R"(
  Frequency        Hello #Ghz
  PeakPower        10000.0 #W
  PulseWidth       0.25 #microsec
  )"};
  RadarConfigParser parser;
  auto config = parser.parseString(config_str);
}

//note PeakPower repeated
void test_repetition() {
  const string config_str = string { R"(
  Frequency        10.0 #Ghz
  PeakPower        10000.0 #W
  PeakPower        10000.0 #W
  PulseWidth       0.25 #microsec
  )"};
  RadarConfigParser parser;
  auto config = parser.parseString(config_str);
}

//note: Bla is not a keyword
void test_text_not_commented() {
  const string config_str = string { R"(
  MaxReceiveTime   1333.33333 #ms
  Bla bla
  BandWidth        4.0 #Mhz
  )"};
  RadarConfigParser parser;
  RadarConfig config = parser.parseString(config_str);
}

void test_parse_file(const string& config_file) {
  RadarConfigParser parser;
  RadarConfig config = parser.parseFile(config_file);
  config.assertParametersSet();
}

void parse_wrong_file() {
  RadarConfigParser parser;
  parser.parseFile("bad_file");  
}


int main(int argc , char ** argv) {

  test_parse_string();
  test_parse_optional_params();
  const string filename = string(argv[1]) + "/radar_configs/short_range_radar.txt";
  test_parse_file( filename );
  
  assertThrow(parse_wrong_file(), invalid_argument);
  assertThrow(test_parse_double_as_integer(), logic_error);
  assertThrow(test_parse_missing_keyword(), logic_error);
  assertThrow(test_parse_wrong_keyword_structure_a(), invalid_argument);
  assertThrow(test_parse_wrong_keyword_structure_b(), invalid_argument);
  assertThrow(test_parse_wrong_keyword_structure_c(), invalid_argument);
  assertThrow(test_repetition(), invalid_argument);
  assertThrow(test_text_not_commented(), invalid_argument);
  return 0;
}
