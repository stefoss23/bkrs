#include <iostream>
#include <vector>
#include <complex>
#include <memory>

#ifndef MATHEMATICS_FOURIER_H
#define MATHEMATICS_FOURIER_H

#include <radsim/mathematics/approx_function.hpp>

// These functions create numerically evaluated fourier transform functions G(t) e.g. F(f) = integral(G(t) * exp(-2*pi*i*t*f) dt)
// Results are either single value or tablefunction.
// Arguments:
// G: the function G(t), either as non-class-bound c++ function or a tablefunction.
// f: argument f in F(f)
// entry The entries, f, in the resulting tablefunction. 
// Inverse: an inverse fourier transform is performed. 

namespace radsim {

template <class T>
std::complex<double>  fourierTransformSingle(const ApproxFunction<T>& G, double f, 
                                             double dt, double t_start, double t_end, bool Inverse = false);

template <class T>
ComplexApproxFunction fourierTransform( const ApproxFunction<T>& G, std::vector<double> entry, 
                                        double dt, double t_start, double t_end, bool Inverse = false);

}

#endif
