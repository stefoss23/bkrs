#include <memory>

#include <radsim/mathematics/constants.hpp>

#include <radsim/radar/bandpass_filter.hpp>

using namespace std;
using namespace std::complex_literals;

namespace radsim {

namespace  {

  //unit
  ComplexApproxFunction standard(double bandwidth)
  //BandWith: hz
  {
    int num_entries = 10000;
    double f_min = - bandwidth; //hz
    double f_max =   bandwidth; //hz
    double df = (f_max - f_min) / (num_entries-1); //hz
    vector<double>          entry(num_entries); //hz
    vector<complex<double>> value(num_entries); //unit

    //Setting leading and trailing edge of filter
    double f_lim1 = 0.25 * bandwidth; //hz
    double f_lim2 = 0.75 * bandwidth; //hz

    for (int n = 0; n < num_entries; n ++) {

      //setting entry frequencies
      double f = f_min + n * df; //hz, entry frequency
      double f_abs = abs(f);  //hz
      entry[n] = f; //hz

      //setting values
      if (f_abs < f_lim1)
        value[n] = 1.0; //unit, banpass peak
      else if (f_abs < f_lim2)
        value[n] = 0.5 * (1 - cos( pi * (f_abs - 0.75 * bandwidth) / (0.5 * bandwidth) )  ); //unit, leading edge
      else
        value[n] = 0.0; //unit, trailing edge
      
    }
    return ComplexApproxFunction(move(entry), move(value)); // func(hz) = unit
  }

  //unit
  ComplexApproxFunction rectangular(double bandwidth) 
  //Bandwidth: hz
  {
    return ComplexApproxFunction({-0.5 * bandwidth, 0.5 * bandwidth}, {1, 1}, 0, 0); // func(hz) = unit
  }

}

//func(hz) = unit
ComplexApproxFunction createBandpassFilter(BandpassFilter type, double bandwidth) 
//bandwidth: hz
{
  switch(type) {
    case BandpassFilter::Standard: return standard(bandwidth); break;
    case BandpassFilter::Rectangular: return rectangular(bandwidth); break;
    default: throw invalid_argument(__PRETTY_FUNCTION__ + string(": type is unknown.")); break;
  }
}

}


