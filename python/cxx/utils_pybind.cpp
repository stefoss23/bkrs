#include <math.h>
#include <iostream>
#include <exception>
#include <complex>

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <radsim/utils/utils.hpp>


using namespace std;
using namespace radsim;
using namespace std::complex_literals;


PYBIND11_MODULE(utils_python, m) {

  m.def("complex_equal", &complex_equal);

}
