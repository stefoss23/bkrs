/*
This struct stores data from a single pulse emission, and contains the necessary
information to carry out target data calculations when combined with radar specs. 
*/

#ifndef RADAR_PULSE_DATA_HPP
#define RADAR_PULSE_DATA_HPP

#include <memory>
#include <vector>

#include <radsim/mathematics/math_vector.hpp>

namespace radsim {

class PulseData {
  private:
    double      t_start;   //s, start time of emission
    math_vector boresight; //unit, boresight position of antennae at emission start 

    PulseData * origin_data;
    unsigned short * origin_reg;

  public:
    PulseData(double t, math_vector boresight_arg, std::vector<unsigned short> registry_arg);

    std::vector<unsigned short> registry; //the resultant samplings per range bin.

    bool isOriginal() const;
    bool hasOriginalRegistry() const;

    double      getStartTime() const;
    math_vector getBoresight() const;
};

}

#endif
