
//Probability Disutribution Functions, will put out a number based on the random generated value, Q
//Q is a value between [0 and 1>.

#ifndef MATHEMATICS_MATHUTILS_HPP
#define MATHEMATICS_MATHUTILS_HPP

namespace radsim {

double rayleighPDF(double x_avg, double Q); //unit as [x_avg]
       //x_avg: var
       //Q: [0, 1>, input to PDF from random number generator

double powerToAmp(double Power); //amp
       //Power: W

void  setRadDefaultRange(double& theta); //theta set in range [0, 2pi>
     //theta: rad

}

#endif
