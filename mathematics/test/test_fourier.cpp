#include <iostream>
#include <vector>
#include <math.h>
#include <assert.h>
#include <complex.h>

#include <radsim/mathematics/constants.hpp>
#include <radsim/mathematics/fourier.hpp>

using namespace std::complex_literals;
using namespace radsim;

std::complex<double> Gauss(double t) {
  return exp(-t * t);
}

//Transform of Gauss yields F = exp(- PI² * f²) * sqrt(PI)
void test_fourier_from_table() {
  //creating the tablefunction G(t)
  int num_entries = 1000;
  std::vector<double>               G_entry(num_entries);
  std::vector<std::complex<double>> G_value(num_entries);
  std::vector<std::complex<double>> H_value(num_entries);
  double t_min = - 3.0;
  double t_max =   3.0;
  double dx = (t_max - t_min) / (double)num_entries;
  for (int n = 0; n < num_entries; n++) {
    double x = t_min + n * dx;
    G_entry[n] = x;
    G_value[n] = Gauss(x);
    
  }
  ComplexApproxFunction G(G_entry, G_value);

  //creating arguments f to F(f) = Fourier(G)
  auto f_entry = std::vector<double>(100);
  double df = 0.01; int n;  double f;
  for(n = 0; n < f_entry.size(); n++) 
    f_entry[n] = n * df;
  
  //The actual Fourier transform
  ComplexApproxFunction F = fourierTransform(G, f_entry, 4e-4, -3.0, 3.0);
  assert( complex_equal(F.output(0.0), sqrt(pi),                        2e-3));
  assert( complex_equal(F.output(0.5), exp(-0.25 * pi * pi) * sqrt(pi), 2e-3));
}

//Transform of ConstFunction from 0.0 to 2.0 yields: -1 / (2 * PI * i * f) * exp(-4 * PI * i * f)
void test_fourier_from_table_const_function() {
  //creating the tablefunction G(t)
  std::vector<double>               G_entry = {0.0, 2.0};
  std::vector<std::complex<double>> G_value = {1.0, 1.0};
  auto G = ComplexApproxFunction(G_entry, G_value);
  
  std::complex<double> result;
  result = fourierTransformSingle(G, 1.0, 1e-4, 0.0, 2.0);
  assert( abs(result) < 1e-3 );
  result = fourierTransformSingle(G, 0.5, 1e-4, 1.0, 2.0);
  std::complex<double> cmp = (2.0i) / pi; 
  assert( complex_equal(result, cmp, 1e-2) );
}



int main() {
  test_fourier_from_table();
  test_fourier_from_table_const_function();
  
  return 0;
}
