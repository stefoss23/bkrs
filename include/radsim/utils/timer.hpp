#include <chrono>
#include <ctime>

#ifndef UTILS_TIMER_HPP
#define UTILS_TIMER_HPP

namespace radsim {

class Timer {

  std::chrono::high_resolution_clock::time_point t_start;

  public:
    Timer();
    double elapsed(); //s, time since initiation of object. 
   
};

}

#endif
