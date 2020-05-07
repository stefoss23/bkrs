#include <iostream>
#include <complex>

#ifndef UTILS_UTILS_HPP
#define UTILS_UTILS_HPP

namespace radsim {

bool double_equal(double d1, double d2, double max_relative_error);
bool complex_equal(std::complex<double> c1, std::complex<double> c2, double max_relative_error);

}

#endif

