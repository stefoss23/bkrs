#include <iostream>
#include <memory>
#include <vector>

#include <radsim/radar/target.hpp>

using namespace std;

namespace radsim {

Target::Target(const math_vector& pos, double rcs) :
  path( VectorApproxFunction(pos) ),
  RCS( rcs )
{
}

Target::Target(VectorApproxFunction path_, double rcs) :
  path( move(path_) ),
  RCS( rcs )
{
}

Target::Target(Target&& other) :
  path( move(other.path) ),
  RCS( other.RCS )
{
}

Target::~Target()
{
}

//m
math_vector Target::getPosition(double t) const
//t: s, time position along path curve
{
  return path.output(t);
}

const VectorApproxFunction& Target::getPath() const {
  return path;
}

void Target::setPosition(const math_vector& pos)
//pos: m
{
  path = VectorApproxFunction(pos);
}

//m2
double Target::getRCS() const
{
  return RCS; //m2  
}

}

