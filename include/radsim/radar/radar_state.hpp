
#include <vector>
#include <list>

#ifndef RADAR_SIM_STATE_HPP
#define RADAR_SIM_STATE_HPP

#include <radsim/mathematics/math_vector.hpp>

namespace radsim {

/*
This struct is used to store target signals from beyond unambiguous range to be
treated in the next pulse emission cycle. 
*/
struct PulseCarry {
  double time; //s
  double power; //s
  math_vector pos; //[m,m,m]

  PulseCarry(double time_arg, double power_arg, const math_vector& pos_arg);

};

/*
A Radar class contains an instance of this class. It is used to store the variable parameters
of the radar during simulation. 
*/
class RadarState
{

  private:
    double time; //s, The 'clock' time of the start of the upcoming pulse
    double theta; //rad, current version of Radar::horizontal_theta, horizontal ant. 
                  //pointing direction

    /*
    The Frame parameters are two unit vectors describing the edges of the antenna frame:

    ^ Frame Y
    |
    |
    *------> FrameX 
    Boresight is pointing out of the 'screen'. 
    */
    math_vector frame_x;
    math_vector frame_y = {0, 0, 1};

    math_vector boresight; //Boresight direction of antennae before upcoming pulse

    std::list<PulseCarry> list_carry; //Results from previous pulse cycles. 
 
    void setAxes();

  public:
    RadarState(double t, double theta_arg);
    //t: s, new start time
    //theta: rad

    void incrementParams(double dt, double dtheta);
    //dt: s
    //dtheta: rad

    void reset(double t, double theta_arg);
    //t: s
    //theta: rad

    double getTime() const; //s
    std::list<PulseCarry>& getListCarry();
    size_t getCarrySize() const;
    const math_vector& getFrameX() const;
    const math_vector& getFrameY() const;
    const math_vector& getBoresight() const; 


};

}

#endif
