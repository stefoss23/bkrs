#include <chrono>
#include <ctime>

#include <radsim/utils/timer.hpp>

using namespace std;
using namespace std::chrono;

namespace radsim {

Timer::Timer() :
  t_start( high_resolution_clock::now() )
{
}

//s
double Timer::elapsed() {
  high_resolution_clock::time_point t_now = high_resolution_clock::now();
  duration<double> time_span = duration_cast<duration<double>>(t_now - t_start);
  return time_span.count();
}

}
