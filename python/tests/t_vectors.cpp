#include <assert.h>

#include <complex>
#include <vector>
#include <iostream>

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <radsim/utils/utils.hpp>

#include "../cxx/convert.hpp"

using namespace std;
using namespace std::complex_literals;
namespace py = pybind11;

typedef array<double, 3> math_vector;


py::array_t<double> send_double_vector() {
  std::vector<double> v = {0.25, 0.125, 0.5};
  return py_convert::numpy_array(v);
}

py::array_t<complex<double>> send_complex_vector() {
  std::vector<std::complex<double>> v(5);
  v[0] = 1.50 + (2.750i);
  v[1] = 1.25 + (4.500i);
  v[2] = 1.00 + (8.250i);
  v[3] = 0.25 + (6.125i);
  v[4] = 8.00 - (9.875i);  
  return py_convert::numpy_array(v);
}


void receive_py_vector_double_empty(py::array_t<double> py_v) {
  std::vector<double> v = py_convert::vector(py_v);
  assert(v.size() == 0);
}

void receive_py_vector_double(py::array_t<double> py_v) {
  vector<double> v = py_convert::vector(py_v);
  assert(v.size() == 3);
  assert( v[0] == 0.625 );
  assert( v[1] == 0.125 );
  assert( v[2] == 0.5   );
} 


void receive_py_vector_ushort(py::array_t<unsigned short> py_v) {
  vector<unsigned short> v = py_convert::vector(py_v);
  cout << endl;
  cout << "Size: " << v.size() << endl;
  cout << "v[0]: " << v[0] << endl;
  cout << "v[1]: " << v[1] << endl;
}

py::array_t<double> send_math_vector() {
  math_vector input = {1.125, 4.750, 0.625};
  return py_convert::numpy_array(input);
}

py::array_t<double> send_around_math_vector(py::array_t<double> input) {
  math_vector r = py_convert::math_vector(input);
  assert(r[0] == 1.250);
  assert(r[1] == 4.625);
  assert(r[2] == 8.000);
  r[0] += 1;
  r[1] += 4;
  r[2] += 8;
  return py_convert::numpy_array(r);
}

py::array send_py_vector_math_vector() {
  math_vector a = {1.50, 2.75, 4.25};
  math_vector b = {3.25, 6.75, 8.00};
  vector<math_vector> v = { a, b };
  return py_convert::numpy_matrix(v);
}

void receive_py_vector_math_vector(py::array input) {
  vector<math_vector> v = py_convert::matrix(input);
  math_vector a = {4.50, 1.75, 2.25};
  math_vector b = {6.25, 8.75, 2.00};
  assert( v[0] == a );
  assert( v[1] == b );
}


PYBIND11_MODULE(test_devel_python, m) {
  m.def("GetDoubleArray", &send_double_vector);
  m.def("SendDoubleArray", &receive_py_vector_double);
  m.def("SendEmptyDoubleArray", &receive_py_vector_double_empty);
  m.def("SendUShortArray", &receive_py_vector_ushort);
  m.def("GetComplexArray", &send_complex_vector);

  m.def("GetMathVector", &send_math_vector);
  m.def("SendAroundMathVector", &send_around_math_vector);
  m.def("GetVectorMathVector", &send_py_vector_math_vector);
  m.def("SendVectorMathVector", &receive_py_vector_math_vector);
}

