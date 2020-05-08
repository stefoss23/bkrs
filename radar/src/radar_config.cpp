#include <math.h>

#include <memory>
#include <exception>
#include <string>

#include <radsim/mathematics/constants.hpp>
#include <radsim/mathematics/mathutils.hpp>

#include <radsim/radar/beam_pattern.hpp>
#include <radsim/radar/radar_config.hpp>


using namespace std;

namespace radsim {

RadarConfig::RadarConfig() {
  adc_converter_mode = ADCMode(-1);
  adc_min2noise = -1;
  adc_max2noise = -1;
  horizontal_beam_shape = BeamPattern::Gaussian;
  elevation_beam_shape = BeamPattern::Gaussian;
  antennae_rotation_speed = 0; //rad/s;
}

void RadarConfig::assertParametersSet() const {
   if (!(peak_power_set &&
         frequency_set &&
         pulse_width_set && 
         prt_set && 
         bandwidth_set &&
         noise_figure_set && 
         duplexer_switch_time_set && 
         hor_beamwidth_set &&
         el_beamwidth_set &&
         antennae_gain_set))
   throw logic_error(__PRETTY_FUNCTION__ + string(": All parameters not set."));

   //testing for logical inconsistencies
   double min_receive_time = pulse_width + duplexer_switch_time;
   double mrt = getMaximumReceiveTime(); //s

   if (prt < min_receive_time)
     throw logic_error(__PRETTY_FUNCTION__ + string(": pulse_repetition_time too short in relation to  minimum_receive_time (switchtime + pulsewidth)."));

   if (mrt < min_receive_time)
     throw logic_error(__PRETTY_FUNCTION__ + string(": maximum_receive_time too short in relation to minimum_receive_time (switchtime + pulsewidth)."));

   if (prt < (mrt + duplexer_switch_time))
     throw logic_error(__PRETTY_FUNCTION__ + string(": pulse_repetition_time too short in relation to  pulse_repetition_time and duplexer_switch_time."));

   if (sampling_time > pulse_width)
     throw logic_error(__PRETTY_FUNCTION__ + string(": Samplingtime must be set to less or equal to pulse_width."));
}

void RadarConfig::setPeakPower(double power)
//power: W
{
  peak_power = power; //W
  peak_power_set = true;
}

void RadarConfig::setFrequency(double f)
//f: Ghz
{
  frequency = 1.0e9 * f; //hz
  frequency_set = true;
}

void RadarConfig::setPulseWidth(double pulsewidth)
//pulsewidth: microsec
{
  pulse_width = pulsewidth * 1e-6; //s
  pulse_width_set = true;
}

void RadarConfig::setSamplingTime(double samplingtime)
//samplingtime: microsec
{
  sampling_time = samplingtime * 1e-6; //s
  sampling_time_set = true;
}

void RadarConfig::setPRT(double t)
//t: ms
{
  prt = t * 1e-3; //s
  prt_set = true;
}

void RadarConfig::setMaximumReceiveTime(double t)
//t: ms
{
  maximum_receive_time = t * 1e-3; //s
  maximum_receive_time_set = true;
}

void RadarConfig::setBandWidth(double bandwidth_arg)
// bandwitdth: Mhz
{
  bandwidth = bandwidth_arg * 1e6; //hz
  bandwidth_set = true;
}

void RadarConfig::setNoiseFigure(double noiseFigure)
//noiseFigure: db, since this is regarded as more front-end.
{
  noise_figure = pow(10, 0.1 * noiseFigure); //unit
  noise_figure_set = true;
}

void RadarConfig::setDuplexerSwitchTime(double switchTime)
//switchTime: microsec 
{
  duplexer_switch_time = switchTime * 1e-6; //s
  duplexer_switch_time_set = true;
}

void RadarConfig::setAntennaeGain(double gain)
//gain: dB
{
  antennae_gain = pow(10, 0.1 * gain); //unit
  antennae_gain_set = true;
}

void RadarConfig::setAzBeamWidth(double horizontalBeamWidth)
//horizontal BeamWidth: degrees, NOT rad
{
  horizontal_beamwidth = pi * horizontalBeamWidth / 180.0;
  hor_beamwidth_set = true;
}

void RadarConfig::setElBeamWidth(double elevationBeamWidth)
//horizontalBeamWidth: //degrees, NOT rad
{
  elevation_beamwidth = pi * elevationBeamWidth / 180.0;
  el_beamwidth_set = true;
}

void RadarConfig::setAntRotSpeed(double speed)
//speed: deg/s
{
  antennae_rotation_speed = - speed * pi / 180.0;
}

void RadarConfig::setAzimuth(double azimuth)
//azimuth: deg
{
  theta = 0.5 * pi - azimuth * pi / 180.0;
  setRadDefaultRange(theta);
  theta_set = true;
}

void RadarConfig::setADCMode(ADCMode adcMode)
{
  adc_converter_mode = adcMode;
}

void RadarConfig::setADCResolution(int resolution)
//resolution: bit
{
  adc_resolution = resolution; //bit
}

void RadarConfig::setADCMax2Noise(double ratio) 
//ratio: unit
{
  adc_max2noise = ratio;
}

void RadarConfig::setADCMin2Noise(double ratio) 
//ratio: unit
{
  adc_min2noise = ratio;
}

//W
double RadarConfig::getPeakPower() const
{
  return peak_power; //W
}

//hz
double RadarConfig::getFrequency() const
{
  return frequency; //hz
}

//s
double RadarConfig::getPulseWidth() const
{
  return pulse_width; //s
}

//s
double RadarConfig::getSamplingTime() const {
  if (sampling_time_set)
    return sampling_time; //s
  else
    return 0.5 * pulse_width;
}

//s
double RadarConfig::getPRT() const {
  return prt; //s
}

//s
double RadarConfig::getMaximumReceiveTime() const
{
  if (maximum_receive_time_set)
    return maximum_receive_time; //s
  else
    return (prt - duplexer_switch_time); //s
}

//hz
double RadarConfig::getBandWidth() const
{
  return bandwidth; //hz
}

//unit, not db
double RadarConfig::getNoiseFigure() const
{
  return noise_figure; //unit
}

//s
double RadarConfig::getDuplexerSwitchTime() const
{
  return duplexer_switch_time; //s
}

//unit
double RadarConfig::getAntennaeGain() const
{
  return antennae_gain;
}

// rad/s
double RadarConfig::getAntRotSpeed() const
{
  return antennae_rotation_speed; //rad/s
}

// rad
double RadarConfig::getTheta() const
{
  if (theta_set)
    return theta; //rad
  else;
    return 0.5 * pi; //rad
}

//rad
double RadarConfig::getHorizontalBeamWidth() const
{
  return horizontal_beamwidth; //rad
}

BeamPattern RadarConfig::getHorizontalBeamShape() const {
  return horizontal_beam_shape;
}

BeamPattern RadarConfig::getElevationBeamShape() const {
  return elevation_beam_shape;
}

//rad
double RadarConfig::getElevationBeamWidth() const {
  return elevation_beamwidth; //rad
}

ADCMode RadarConfig::getADCMode() const {
  return adc_converter_mode;
}

//bit
int RadarConfig::getADCResolution() const
{
  return adc_resolution;
}

//unit
double RadarConfig::getADCMax2Noise() const
{
  return adc_max2noise; //unit
}

//unit
double RadarConfig::getADCMin2Noise() const
{
  return adc_min2noise; //unit
}

} //end namespace bkradsim
