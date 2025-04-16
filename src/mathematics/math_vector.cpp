#include <math.h>

#include <string>
#include <iostream>

#include <radsim/mathematics/math_vector.hpp>

using namespace std;

namespace radsim {

double operator * (const math_vector& a, const math_vector& b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

math_vector operator * (const math_vector& a, double c) {
  return {a[0] * c, a[1] * c, a[2] * c};
}

math_vector operator * (double c, const math_vector& a) {
  return a * c;
}

math_vector operator + (const math_vector& a, const math_vector& b) {
  return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}

math_vector operator / (const math_vector& v, double s) {
  return {v[0] / s, v[1] / s, v[2] / s};
}

double math_vector_length(const math_vector& v) {
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

//rad
double math_vector_angle(const math_vector& a, const math_vector& b) {
  return acos( (a * b) / (math_vector_length(a) * math_vector_length(b) ) );
}

math_vector math_vector_unit(const math_vector& v) {
  return v / math_vector_length(v);
}

void operator /= (math_vector& a, double c) {
  a[0] /= c;
  a[1] /= c;
  a[2] /= c;
}

void operator *= (math_vector& a, double c) {
  a[0] *= c;
  a[1] *= c;
  a[2] *= c;
}

math_vector operator - (const math_vector& a, const math_vector& b) {
  return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}

string to_string(const math_vector& v) {
  string str = "[ ";
  str += std::to_string(v[0]);
  str += " , ";
  str += std::to_string(v[1]);
  str += " , ";
  str += std::to_string(v[2]);
  str += " ]";
  return str;
}

void math_vector_set_unit(math_vector& v) {
  double l = math_vector_length(v);
  v /= l;  
}

bool math_vector_equal(const math_vector& a, const math_vector& b, double relative_error) {
  double la = math_vector_length(a);
  double lb = math_vector_length(b);
  double max_error = la;
  if (lb > la)
    max_error = lb;

  max_error *= relative_error;

  math_vector error_vector = a - b;
  double error = math_vector_length(error_vector);
  
  if (error <= max_error)
    return true;
  else
    return false;
}


void math_vector_rotate_xy( math_vector& v, double angle )
//angle: rad
{
  double x = v[0];
  double y = v[1];
  double cos_ = cos(angle);
  double sin_ = sin(angle);
   
  v[0] = x * cos_ - y * sin_;
  v[1] = x * sin_ + y * cos_;    
}

//spherical, returns a vector r * [cos(phi) * cos(theta), cost(phi) * sin(theta), sin(phi)]
math_vector math_vector_spherical(double r, double phi, double theta) {
  double r_ = r * cos(phi);
  return {r_ * cos(theta), r_ * sin(theta), r * sin(phi)};
}

}

