#include <vector>
#include <array>
#include <complex>

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include  "convert.hpp"

#include <radsim/mathematics/mathutils.hpp>
#include <radsim/mathematics/approx_function.hpp>
#include <radsim/mathematics/math_vector.hpp>
#include <radsim/mathematics/fourier.hpp>


using namespace std;
using namespace std::complex_literals;
using namespace radsim;
namespace py = pybind11;


namespace {

//Approx Functions
template <class T>
py::array_t<T> ApproxFunctionType_Py_Call(const ApproxFunction<T>& F, py::array_t<double> py_input) {
  vector<double> input = py_convert::vector(py_input);
  return py_convert::numpy_array( F.outputVector(input) );
}

template <class T>
py::array_t<double> ApproxFunctionType_Py_Entry(const ApproxFunction<T>& F) {
  return py_convert::numpy_array( F.getEntryVector() );
}

template <class T>
py::array_t<T> ApproxFunctionType_Py_Value(const ApproxFunction<T>& F) {
  return py_convert::numpy_array( F.getValueVector() );
}

//End approx functions

}

PYBIND11_MODULE(mathematics_python, m) {

  //MathUtils
  m.def("rayleigh", &rayleighPDF);
  m.def("power_to_amp", &powerToAmp);


  //Fourier transforms
  m.def("fourier_transform", [](const ComplexApproxFunction& G, double f, double dt, double begin, double end, bool inv) -> complex<double> {
     return fourierTransformSingle(G, f, dt, begin, end, inv);
   } );

  m.def("fourier_transform", [](const ComplexApproxFunction& G, py::array_t<double> py_f_entry, double dt, double begin, double end, bool inv) 
  -> ComplexApproxFunction {
     vector<double> f_entry = py_convert::vector(py_f_entry);    
     return fourierTransform(G, move(f_entry), dt, begin, end, inv);
   } );


  //DoubleApproxFunction
  py::class_<DoubleApproxFunction> (m, "DoubleApproxFunction")
  .def(py::init([](py::array_t<double> py_entry, py::array_t<double> py_value, double value_begin, double value_end) {
       vector<double> entry = py_convert::vector(py_entry);
       vector<double> value = py_convert::vector(py_value);
       return DoubleApproxFunction(move(entry), move(value), value_begin, value_end);
     }  ), py::arg("py_entry"), py::arg("py_value"), py::arg("value_0") = 0, py::arg("value_1") = 0  )

  .def("__call__", [](const DoubleApproxFunction& F, double x) -> double {
       return F.output(x);
     } )

  .def("__call__", [](const DoubleApproxFunction& F, py::array_t<double> py_input) -> py::array_t<double> {
       return ApproxFunctionType_Py_Call(F, py_input);
     } )

  .def_property_readonly("entry", &ApproxFunctionType_Py_Entry<double>)
  .def_property_readonly("value", &ApproxFunctionType_Py_Value<double>);



  //ComplexApproxFunction
  py::class_<ComplexApproxFunction> (m, "ComplexApproxFunction")
  .def(py::init([](py::array_t<double> py_entry, py::array_t<complex<double>> py_value, complex<double> value_begin, complex<double> value_end) {
       vector<double> entry          = py_convert::vector(py_entry);
       vector<complex<double>> value = py_convert::vector(py_value);
       return ComplexApproxFunction(move(entry), move(value), value_begin, value_end);
     }  ), py::arg("py_entry"), py::arg("py_value"), py::arg("value_0") = 0, py::arg("value_1") = 0  )

  .def("__call__", [](const ComplexApproxFunction& F, double x) -> complex<double> {
       return F.output(x);
     } )

  .def("__call__", [](const ComplexApproxFunction& F, py::array_t<double> py_input) -> py::array_t<complex<double>> {
       return ApproxFunctionType_Py_Call(F, py_input);
     } )

  .def_property_readonly("entry", &ApproxFunctionType_Py_Entry<complex<double>>)
  .def_property_readonly("value", &ApproxFunctionType_Py_Value<complex<double>>);



  //MathVectorApproxFunction
  py::class_<VectorApproxFunction> (m, "MathVectorApproxFunction")
  .def(py::init([](py::array_t<double> py_entry, py::array py_value) {
       vector<double> entry          = py_convert::vector(py_entry);
       vector<math_vector> value     = py_convert::matrix(py_value);
       return VectorApproxFunction(move(entry), move(value) );
     }  )  )

  .def_property_readonly("value", [](const VectorApproxFunction& F) -> py::array {
       return py_convert::numpy_matrix( F.getValueVector() );
     } )

  .def("__call__", [](const VectorApproxFunction& F, double x) -> py::array_t<double> {
       return py_convert::numpy_array( F.output(x) );
     } )

  .def("__call__", [](const VectorApproxFunction& F, py::array_t<double> py_input) -> py::array {
       vector<double> input = py_convert::vector(py_input);
       return py_convert::numpy_matrix( F.outputVector(input) );
     } )

  .def_property_readonly("entry", &ApproxFunctionType_Py_Entry<math_vector>);


}
