#include <chrono>
#include <ctime>

#include <radsim/utils/assert.hpp>
#include <radsim/utils/timer.hpp>
#include <radsim/utils/utils.hpp>

using namespace std;
using namespace std::chrono;
using namespace radsim;


double get_time_elapsed(high_resolution_clock::time_point t_start) {
  high_resolution_clock::time_point t_now = high_resolution_clock::now();
  duration<double> time_span = duration_cast<duration<double>>(t_now - t_start);
  return time_span.count();
}


void test_timer() {

  double test_time = 1.0;

  high_resolution_clock::time_point t_start = high_resolution_clock::now();
  Timer timer;  
  while (timer.elapsed() < test_time) {}
  double time_elapsed = get_time_elapsed(t_start);

  assertDoubleEqual( get_time_elapsed(t_start), test_time, 1e-2 );
  

}


int main() {
  test_timer();
  return 0;
}
