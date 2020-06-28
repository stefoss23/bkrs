
#include <vector>
#include <array>
#include <string>

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#ifndef PYTHON_BINDINGS_VECTOR_HPP
#define PYTHON_BINDINGS_VECTOR_HPP


namespace py_convert {

template <class T>
pybind11::array_t<T> numpy_array(const std::vector<T>& input) {
  auto output =  pybind11::array_t<T>(input.size());
  T * py_array_ptr = (T*)output.request().ptr;

  for (int i = 0; i < input.size(); i++)
    py_array_ptr[i] = input[i];

  return output;
}


template <class T>
std::vector<T> vector(pybind11::array_t<T>& input) {
  T * input_ptr    = (T *) input.request().ptr; 
  std::vector<T> output(input.size());
  
  for (int i = 0; i < input.size(); i++)
    output[i] = input_ptr[i];

  return output;
}


pybind11::array_t<double> numpy_array(const std::array<double, 3>& input) {
  int size = 3;
  auto output =  pybind11::array_t<double>(size);
  double * py_array_ptr = (double*)output.request().ptr;

  for (int i = 0; i < 3; i++)
    py_array_ptr[i] = input[i];

  return output;
}


std::array<double, 3> math_vector(pybind11::array_t<double>& input) {
  
  int size = input.size();
  if (size != 3)
    throw std::invalid_argument(__PRETTY_FUNCTION__ + std::string(": numpy array does not have length == 3.") );

  double * py_array_ptr = (double*)input.request().ptr;
  std::array<double, 3> output;

  for (int i = 0; i < 3; i++)
    output[i] = py_array_ptr[i];

  return output;
}


pybind11::array numpy_matrix(const std::vector< std::array<double, 3> >& v) {

  int ndim = 2;
  int size = v.size();
  std::vector<ssize_t> shape = {size, 3};
  std::vector<ssize_t> strides = { sizeof(double)*3 , sizeof(double) };

  std::vector<double> result(size * 3);

  for (int i = 0; i < size; i++) {
    result[3 * i    ] = v[i][0];
    result[3 * i + 1] = v[i][1];
    result[3 * i + 2] = v[i][2];
  }

  pybind11::array output = pybind11::array(pybind11::buffer_info(
    result.data(),                           /* data as contiguous array  */
    sizeof(double),                          /* size of one scalar        */
    pybind11::format_descriptor<double>::format(), /* data type                 */
    ndim,                                    /* number of dimensions      */
    shape,                                   /* shape of the matrix       */
    strides                                  /* strides for each axis     */
  ));

  return output;
}


std::vector< std::array<double, 3> > matrix(pybind11::array& input) {

  int m = input.shape()[0];
  int n = input.shape()[1];

  double * ptr = (double*)input.request().ptr;

  std::vector< std::array<double, 3> > output(m);

  int counter = 0;
  
  for (int i = 0; i < m; i++)
    for (int j = 0; j < n; j++) {
      output[i][j] = ptr[counter];
      counter++;
    }

  return output;
}


}

#endif
