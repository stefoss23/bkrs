#include <assert.h>
#include <stdio.h>

#include <iostream>

#include <radsim/utils/rng.hpp>
#include <radsim/utils/utils.hpp>

using namespace std;
using namespace radsim;

double random_func1(unsigned int * seed) {
  return (double)rand_r(seed) / (double)RAND_MAX;
}

double random_func2(unsigned int * seed) {
  unsigned int max = 4;
  double returnvalue = *seed * 0.25;
  *seed = (*seed+1) % max;
  return returnvalue;
}

void test_rng_default() {
  RNG g;
  assert( g.output() != g.output() );
  assert( g.output() >= 0.0 );
  assert( g.output() <  1.0 );
}

//Compared with output form the c-lib rand_r function
void test_rng_seeded(unsigned int seed) {
  unsigned int old_seed = seed;
  double exp_result = (double)rand_r(&seed) / (double)RAND_MAX;
  RNG g(true, old_seed);
  double result = g.output();
  assert( double_equal(result, exp_result, 1e-6) );
  assert( g.getSeed() == seed );
}

void test_rng_set_rand_r(unsigned int seed) {
  RNG g(true, seed, &random_func1);
  assert( g.output() == random_func1(&seed) );
}

void test_rng_custom() {
  unsigned int seed = 0;
  RNG g(true, seed, &random_func2);
  assert( g.output() == 0.00 );
  assert( g.output() == 0.25 );
  assert( g.output() == 0.50 );
  assert( g.output() == 0.75 );
  assert( g.output() == 0.00 );
}

int main() {

  test_rng_default();
  test_rng_seeded(512);
  test_rng_set_rand_r(435);
  test_rng_custom();

  return 0;
}
