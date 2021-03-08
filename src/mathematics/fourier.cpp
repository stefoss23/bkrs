#include <complex.h>
#include <iostream>
#include <memory>

#include <radsim/mathematics/constants.hpp>
#include <radsim/mathematics/fourier.hpp>

using namespace std;
using namespace std::complex_literals;

namespace radsim {

// These functions create numerically evaluated fourier transform functions of F(f) = integral(G(t) * exp(-2*pi*i*t*f) dt)
// Results are either single value or tablefunction.
// Arguments:
// G: the function G(t)
// f: argument f in F(f)
// entry The entries, f, in the resulting tablefunction.


template <class T>
complex<double> fourierTransformSingle(const ApproxFunction<T>& G, double f, 
                                       double dt, double t_start, double t_end, bool Inverse) {
  complex<double> F = 0;
  double t;
  double Sign = -1;
  if (Inverse)
    Sign = 1;
  for (t = t_start; t < t_end; t += dt)
    F += G.output(t) * exp(Sign * 2 * pi * 1i * t * f);
  F *= dt;
  return F;
}

template complex<double> fourierTransformSingle(const DoubleApproxFunction& G, double f, 
                                                double dt, double t_start, double t_end, bool Inverse);

template complex<double> fourierTransformSingle(const ComplexApproxFunction& G, double f, 
                                                double dt, double t_start, double t_end, bool Inverse);

template <class T>
ComplexApproxFunction fourierTransform( const ApproxFunction<T>& G, vector<double> entry, 
                                        double dt, double t_start, double t_end, bool Inverse) {
  vector<complex<double> > value(entry.size());

  for (int n = 0; n < entry.size(); n++)
     value[n] = fourierTransformSingle(G, entry[n], dt, t_start, t_end, Inverse);
  return ComplexApproxFunction( move(entry), move(value) );    
}

template ComplexApproxFunction fourierTransform( const DoubleApproxFunction& G, vector<double> entry, 
                                                 double dt, double t_start, double t_end, bool Inverse);

template ComplexApproxFunction fourierTransform( const ComplexApproxFunction& G, vector<double> entry, 
                                                 double dt, double t_start, double t_end, bool Inverse);

}



