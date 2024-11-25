#include <vector>
#include <memory>
#include <thread>

#include <radsim/utils/assert.hpp>

#include <radsim/mathematics/math_vector.hpp>

#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/radar_data_queue.hpp>

using namespace std;
using namespace radsim;


void slow_function() {
  for (int i = 0; i < 10000; i++)
    double j = i * 10.2 + 0.67;
}

void push_runner(RadarDataQueue * queue, bool * slow_push, bool * on) {

  math_vector boresight = {1, 0, 0};
  double sim_time = -0.9;
  double dt = 0.1;
  while(*on) {
    if (*slow_push)
      slow_function();
    queue->push( PulseData(sim_time, boresight, vector<unsigned short>(0)) );
    sim_time += dt;
  }

}

void test_concurrence(bool slow_pop, bool slow_push) {
  RadarDataQueue queue;
  bool on = true;

  queue.pushInitial( PulseData(-1.0, (math_vector){1.0, 1.0, 1.0}, vector<unsigned short>(0)) );

  thread push_thread(push_runner, &queue, &slow_push, &on);

  int data_count = 0;
  int max_count = 1000;

  double sim_time = -1.1;
  while (data_count < max_count) {
    if (queue.size() > 1) {
      auto packet = queue.pop();
      data_count++;
      double old_time = sim_time;
      sim_time = packet.getStartTime();
      assertDoubleEqual(  sim_time - old_time, 0.1, 1e-4);
    }
    if (slow_pop)
      slow_function();
  }
  on = false;
  push_thread.join();
}


int main() {
  test_concurrence(false, false);
  test_concurrence(false, true);
  test_concurrence(true, false);
  test_concurrence(true, true);
  return 0;
}
