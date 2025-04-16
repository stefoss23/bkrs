#include <stdio.h>error
#include <stdlib.h>
#include <time.h>

#include <exception>
#include <iostream>

#include <radsim/utils/rng.hpp>

using namespace std;

namespace {

double Q_default(unsigned int * seed) {
  return (double)rand_r(seed) / (double)RAND_MAX;
}

}

namespace radsim {

void RNG::set_default_seed() {
  time_t t;
  seed = (unsigned) time(&t);
}

void RNG::set_default_Q() {
  if (RAND_MAX < 1000000000) 
  {
    cout << "ERROR in: " << __PRETTY_FUNCTION__ << ", system defined value of RAND_MAX is too low." << endl;
    abort();
  }
  Q = &Q_default;
}

RNG::RNG(bool custom, int seed_value, double (*Q_custom)(unsigned int *)) {

  if (custom)
    seed = seed_value;
  else
    set_default_seed();

  if (Q_custom == NULL)
    set_default_Q();
  else
    Q = Q_custom;
}


double RNG::output() {
  return Q(&seed);
}

unsigned int RNG::getSeed() {
  return seed;
}

}
