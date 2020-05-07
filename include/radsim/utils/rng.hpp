#include <memory>

#ifndef UTILS_RNG_HPP
#define UTILS_RNG_HPP

namespace radsim {

/*
Random number generator, using the c-lib stdlib.h function rand_r.
The seed is kept track of by the object.
*/
class RNG {

  unsigned int seed;
  double (*Q)(unsigned int *);  //[0, 1>: The randomizing function, taking 'seed' as input. 

  void set_default_seed();
  void set_default_Q();

  public:
    RNG(bool custom = false, int seed_value = 0, double (*Q_custom)(unsigned int *) = NULL);
    //Q_custom: the custom randomizing function. 
    //if Q_custom==NULL, Q = rand_r. 

    double output();
    unsigned int getSeed();

};

}

#endif
