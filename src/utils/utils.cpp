#include <math.h>
#include <iostream>
#include <exception>
#include <complex>

#include <radsim/utils/utils.hpp>

using namespace std;

namespace radsim {

bool complex_equal(complex<double> c1, complex<double> c2, double max_relative_error) {
    if (c1 == 0.0 && c2 == 0.0)
      return true;
    double base = 0;
    if (c1 == 0.0)
      base = abs(c2);
    else if (abs(c1) <= abs(c2))
      base = abs(c1);
    else
      base = abs(c2);
    double max_error = base * max_relative_error;
    double error = abs(c2 - c1);
    if (error <= max_error)
      return true;
    else
      return false;
}

bool double_equal(double d1, double d2, double max_relative_error) {
    return complex_equal( d1, d2, max_relative_error );
}

}




