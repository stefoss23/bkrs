
#ifndef TARGETS_TARGET_HPP
#define TARGETS_TARGET_HPP

#include <iostream>
#include <memory>
#include <list>

#include <radsim/mathematics/math_vector.hpp>
#include <radsim/mathematics/approx_function.hpp>

namespace radsim {

class Target
{

  VectorApproxFunction path; //func(s) = [m, m, m]
  double RCS;

  public:
    Target(const math_vector& pos, double rcs);
    //x: m
    //rcs: m2

    Target(VectorApproxFunction path, double rcs);
    //path: func(s) = [m, m, m]
    //rcs_: m2

    Target(Target&& other);
    Target(const Target& other) = default;
    Target& operator=(const Target& other) = default;

    ~Target();

    math_vector getPosition(double t = 0) const; //m
    //t: s, time position along path curve

    const VectorApproxFunction& getPath() const; //func(s) = [m, m, m]

    double getRCS() const; //m2

    void   setPosition(const math_vector& pos);
    //pos: m
    
};

typedef std::list<Target> TargetCollection;

}

#endif
