#include <iostream>


#ifndef RADAR_ADC_HPP
#define RADAR_ADC_HPP

namespace radsim {

enum class ADCMode { Power, Logarithm };

//Analog to Digital Conversion class
class ADC
{
  private:
    ADCMode mode;
    double minimum_power;   //W, the minimum possible power measured, if any input is 
                           //lower, convert output is 0'
    double maximum_power;  //the maxiumum possible measured, if any input is max or higher, 
                           //converter output is num_levels-1
    double log_constant;    //unit, used for calculations
    int    num_levels;      //typical: 6 to 16 bits (64 to 65000 levels) 

    unsigned short convertPower(double power) const; //unit
    //power: W

    unsigned short convertLogarithmic(double power) const; //unit
    //Power: W

    void setLogarithmicMinimumPower(double power);
    //Power: W

  public:
    ADC(int resolution, ADCMode mode, double min_power, double max_power = 0);
    //resolution: bits
    //min_power: W
    //max_power: W

    ~ADC();

    int getNumLevels() const;
    double getSensitivity() const; //W

    unsigned short convertSignal(double power) const; //unit
    //power: W
};

}

#endif
