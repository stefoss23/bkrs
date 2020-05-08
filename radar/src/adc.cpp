#include <math.h>
#include <iostream>
#include <cstdlib>
#include <exception>

#include <radsim/radar/adc.hpp>

using namespace std;

namespace radsim {

ADC::ADC(int resolution, ADCMode mode_, double min_power, double max_power)
    //resolution: bits
    //min_power: W
    //max_power: W
{
  if (resolution > 16)
    throw invalid_argument(__PRETTY_FUNCTION__ + string(": Resolution argument too high. Maximum resolution is 16-bit."));

  if (min_power <= 0)
    throw invalid_argument(__PRETTY_FUNCTION__ + string(": minimum power must be greater than 0 W."));

  minimum_power = min_power;
  num_levels = pow(2, resolution);
  mode = mode_;
  switch(mode)
  {
    case ADCMode::Power:
      maximum_power = (num_levels-1) * minimum_power; //W
      break;
    case ADCMode::Logarithm: 
      if (min_power >= max_power)
        throw invalid_argument(__PRETTY_FUNCTION__ + string(": minimum power must be less than maximum power."));
      maximum_power = max_power;
      log_constant = (num_levels - 1) / log(maximum_power / minimum_power); //unit
      break;
    default:
      throw invalid_argument(__PRETTY_FUNCTION__ + string(": ") + string(": not an allowed mode.")); 
      break;
  }
}

ADC::~ADC() {
}

int ADC::getNumLevels() const {
  return num_levels;
}

//W
double ADC::getSensitivity() const {
  return minimum_power; //W
}

//unit
unsigned short ADC::convertPower(double amplitude) const
//amplitude: amp
{
  double power = amplitude * amplitude; //W
  if (power >= maximum_power)
    return (num_levels - 1); //unit

  return (unsigned short) (power / minimum_power); //unit
}

//unit
unsigned short ADC::convertLogarithmic(double amplitude) const
//Amplitude: amp
{
  double power = amplitude * amplitude;
  if (power >= maximum_power)
    return (num_levels - 1); //unit
  if (power < minimum_power)
    return 0; //unit
    
  return (unsigned int) log_constant * log(power / minimum_power); //unit
}

//unit
unsigned short ADC::convertSignal(double amplitude) const
//Amplitude: amp
{
  switch(mode)
  {
    case ADCMode::Power:
      return convertPower(amplitude); //unit
    case ADCMode::Logarithm:
      return convertLogarithmic(amplitude); //unit
    default:
      return 0; //unit
  }
}

} //end namespace bkradsim
