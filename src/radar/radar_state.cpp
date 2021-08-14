#include <math.h>

#include <vector>

#include <radsim/radar/radar_state.hpp>

using namespace std;

namespace radsim {

PulseCarry::PulseCarry(double time_arg, double power_arg, const math_vector& pos_arg) :
  time(time_arg),
  power(power_arg),
  pos(pos_arg)
{}


RadarState::RadarState(double t, double theta_arg) :
  time(t),
  theta(theta_arg)
{
  setAxes();
}

void RadarState::setAxes() {
  frame_x =   {-sin(theta), cos(theta), 0}; //unit
  boresight = { cos(theta), sin(theta), 0}; //unit
}

void RadarState::incrementParams(double dt, double dtheta)
//dt: s
//dtheta: rad
{
  time += dt;
  theta += dtheta;
  setAxes();
}


void RadarState::reset(double t, double theta_arg) {
  time = t;
  theta = theta_arg;
  setAxes();
  list_carry.clear();
}

//s
double RadarState::getTime() const {
  return time; //s
}

std::list<PulseCarry>& RadarState::getListCarry() {
  return list_carry;
}

size_t RadarState::getCarrySize() const {
  return list_carry.size();
}

const math_vector& RadarState::getFrameX() const {
  return frame_x;
}

const math_vector& RadarState::getFrameY() const {
  return frame_y;
}

const math_vector& RadarState::getBoresight() const {
  return boresight;
}

double RadarState::getTheta() const {
  return theta;
}

} //end namespace bkradsim
