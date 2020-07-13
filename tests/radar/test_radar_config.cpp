#include <assert.h>
#include <math.h>

#include <memory>
#include <exception>

#include <radsim/utils/utils.hpp>
#include <radsim/utils/test_utils.hpp>

#include <radsim/mathematics/constants.hpp>

#include <radsim/radar/radar_config.hpp>


using namespace std;
using namespace radsim;

void test_init() {
  RadarConfig Config;

  Config.setPeakPower(1.1);
  Config.setFrequency(10.0); //input in Ghz
  Config.setPulseWidth(2.2);
  Config.setPRT(50.0);
  Config.setMaximumReceiveTime(30.3);
  Config.setBandWidth(4.4);
  Config.setNoiseFigure(20.0); //input in db
  Config.setDuplexerSwitchTime(6.6);
  Config.setAzBeamWidth(180.0); //input in degrees
  Config.setElBeamWidth(90.0); //input in degrees
  Config.setAntRotSpeed(180.0); //input in degrees/s
  Config.setAntennaeGain(20.0);
  Config.setHorizontalBeamShape(BeamPattern::Triangular);
  Config.setElevationBeamShape(BeamPattern::Triangular);
  Config.assertParametersSet();
  assert( double_equal(1.1, Config.getPeakPower(), 1e-3) );
  assert( double_equal(1.0e10, Config.getFrequency(), 1e-3) );
  assert( double_equal(2.2e-6, Config.getPulseWidth(), 1e-3) );
  assert( double_equal(50.0e-3, Config.getPRT(), 1e-3) );
  assert( double_equal(30.3e-3, Config.getMaximumReceiveTime(), 1e-3) );
  assert( double_equal(4.4e6, Config.getBandWidth(), 1e-3) );
  assert( double_equal(100.0, Config.getNoiseFigure(), 1e-3) ); //output in unit
  assert( double_equal(6.6e-6, Config.getDuplexerSwitchTime(), 1e-3) );
  assert( double_equal(100.0, Config.getAntennaeGain(), 1e-3) );
  assert( double_equal(pi, Config.getHorizontalBeamWidth(), 1e-3) ); //output in rad
  assert( double_equal(pi/2, Config.getElevationBeamWidth(), 1e-3) ); //output in rad
  assert( double_equal(-pi, Config.getAntRotSpeed(), 1e-3) ); //output in rad/s
  assert( BeamPattern::Triangular == Config.getHorizontalBeamShape() );
  assert( BeamPattern::Triangular == Config.getElevationBeamShape() );
}

void test_wrong_sampling_time()
{
  RadarConfig Config;
  Config.setPeakPower(1.1);
  Config.setFrequency(10.0); //input in Ghz
  Config.setPulseWidth(2.2);
  Config.setMaximumReceiveTime(30.3);
  Config.setBandWidth(4.4);
  Config.setNoiseFigure(20.0); //input in db
  Config.setDuplexerSwitchTime(6.6);
  Config.setAzBeamWidth(180.0); //input in degrees
  Config.setAntRotSpeed(180.0); //input in degrees/s
  Config.setAntennaeGain(20.0);
  Config.setSamplingTime(2.3);
  Config.assertParametersSet();
}


int main() {
  test_init();
  assert_throw<logic_error>(&test_wrong_sampling_time);
}
