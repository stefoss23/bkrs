#include <math.h>

#include <iostream>
#include <vector>

#include <radsim/mathematics/approx_function.hpp>

#include <radsim/radar/beam_pattern.hpp>

using namespace std;

namespace radsim {

//func(rad)=unit
static DoubleApproxFunction TriangularPattern(double beamwidth)
//func(rad)=unit
  {
    return DoubleApproxFunction({-beamwidth, 0.0, beamwidth},
                               (vector<double>){0.0, 1.0, 0.0}); //func(rad) = unit
  }


//func(rad)=unit
static DoubleApproxFunction GaussianPattern(double beamwidth)
//func(rad)=unit
  {
    int NumValues = 1000;
    vector<double> entry(NumValues);
    vector<double> value(NumValues);
    double dt = 4 * beamwidth / (NumValues - 1); //rad
    double t_min = - 2.0 * beamwidth; //rad
    double k = 4.0 * log(2.0) / (beamwidth * beamwidth);
    for (int i = 0; i < NumValues; i++) {
      double t = t_min + dt * i; //rad
      entry[i] = t;
      value[i] = exp( - k * t * t);
    }
    return DoubleApproxFunction(entry, value);
  }



//func(rad)=unit
DoubleApproxFunction createBeamPattern(BeamPattern Shape, double beamwidth)
//beamwidth: rad
  {
    DoubleApproxFunction BeamPattern(0);
    switch(Shape) {
      case BeamPattern::Triangular: BeamPattern = TriangularPattern(beamwidth); break;
      case BeamPattern::Gaussian:   BeamPattern = GaussianPattern(beamwidth); break;
      default:
        throw invalid_argument(__PRETTY_FUNCTION__ + string(": ") + string(": not an allowed mode.")); 
        break;
    }
    return BeamPattern;  
  } 

}
