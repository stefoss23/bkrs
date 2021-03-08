#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <exception>

#include <radsim/utils/utils.hpp>

#include <radsim/mathematics/constants.hpp>
#include <radsim/mathematics/mathutils.hpp>

namespace radsim {

//unit as [x_avg]
double rayleighPDF(double x_avg, double Q)
//x_avg: var
//Q: [0, 1>, input to PDF from random number generator
  {
    if (Q >= 1 || Q < 0) 
    {     
      throw std::invalid_argument(__PRETTY_FUNCTION__ + std::string(": Input variable Q must be in interval [0, 1>"));
    }
    
    return -x_avg * log(1 - Q);
  }

//amp
double powerToAmp(double Power)
//Power: W
  {
    return sqrt(Power);
  }

void setRadDefaultRange(double& theta)
//theta: rad
{
  double c = 2 * pi;
  while(theta < 0) theta  += c;
  while(theta >= c) theta -= c;
}

}
