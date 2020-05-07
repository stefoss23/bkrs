#include <array>
#include <string>

#ifndef MATHEMATICS_MATH_VECTOR_H
#define MATHEMATICS_MATH_VECTOR_H

namespace radsim {

typedef std::array<double, 3> math_vector;

double      operator * (const math_vector& a, const math_vector& b);
math_vector operator * (const math_vector& a, double c);
math_vector operator * (double c, const math_vector& a);
math_vector operator / (const math_vector& v, double s);
math_vector operator + (const math_vector& a, const math_vector& b);
math_vector operator - (const math_vector& a, const math_vector& b);

void operator /= (math_vector& a, double c);
void operator *= (math_vector& a, double c);

double      math_vector_length(const math_vector& v);
double      math_vector_angle(const math_vector& a, const math_vector& b); //rad
math_vector math_vector_unit(const math_vector& v);
std::string to_string(const math_vector& v);
void        math_vector_set_unit(math_vector& v);
bool        math_vector_equal(const math_vector& a, const math_vector& b, double relative_error); 


//transforms
void math_vector_rotate_xy( math_vector& v, double angle );
//angle: rad

//spherical, returns a vector r * [cos(phi) * cos(theta), cost(phi) * sin(theta), sin(phi)]
math_vector math_vector_spherical(double r, double phi, double theta);

}

#endif
