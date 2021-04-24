
#include <assert.h>

#include <vector>
#include <memory>
#include <thread>

#include <radsim/utils/test_utils.hpp>

#include <radsim/utils/utils.hpp>

#include <radsim/mathematics/math_vector.hpp>

#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/radar_data_queue.hpp>

using namespace std;
using namespace radsim;

typedef vector<unsigned short> registry;

void test_queue() {

  math_vector boresight = {1, 0, 0};

  registry s1 = {1};
  registry s2 = {1, 2};
  registry s3 = {1, 2, 3};
  registry s4 = {1, 2, 3, 4};
  registry s5 = {1, 2, 3, 4, 5};
  registry s6 = {1, 2, 3, 4, 5, 6};

  unsigned short * ptr1 = s1.data();
  unsigned short * ptr2 = s2.data();
  unsigned short * ptr3 = s3.data();
  unsigned short * ptr4 = s4.data();

  RadarDataQueue queue;

  assert( queue.isEmpty() );
  assert( queue.size() == 0 );
  queue.pushInitial( PulseData(1.0, boresight, move(s1)) );  // ----- queue = {p1}
  assert( !queue.isEmpty() );
  assert( queue.size() == 1 );
  queue.push( PulseData(2.0, boresight, move(s2)) );  // ----- queue = {p1, p2}
  assert( queue.size() );
  auto pkg_1 = queue.pop(); // ---- queue = {p2}
  assert( pkg_1.registry.data() == ptr1 );
  assert( pkg_1.hasOriginalRegistry() );
  assert( queue.size() == 1 );
  assert( !queue.isEmpty() );
  queue.push( PulseData(3.0, boresight, move(s3)) ); // ------ queue = {p2, p3}
  assert( queue.size() == 2 );
  auto pkg_2 = queue.pop(); // ---- queue = {p3}
  assert( pkg_2.registry.data() == ptr2 );
  assert( queue.size() == 1);
  queue.push( PulseData(4.0, boresight, move(s4)) ); // ------ queue = {p3, p4}
  assert( !queue.isEmpty() );
  assert( queue.size() == 2 );
  queue.push( PulseData(5.0, boresight, move(s5)) ); // ------ queue = {p3, p4, p5}
  assert( queue.size() == 3 );
  queue.push( PulseData(6.0, boresight, move(s6)) ); // ------ queue = {p3, p4, p5, p6}
  auto pkg_3 = queue.pop(); // ---- queue = {p4, p5, p6}
  assert( pkg_3.registry.data() == ptr3 );
  assert( queue.size() == 3);
  auto pkg_4 = queue.pop(); // ---- queue = {p5, p6}
  assert( pkg_4.registry.data() == ptr4 );
  assert( queue.size() == 2);

}

void test_empty() {
  math_vector boresight = {1, 0, 0};

  registry s1 = {1};
  registry s2 = {1, 2};
  registry s3 = {1, 2, 3};

  RadarDataQueue queue;
  queue.pushInitial( PulseData(1.0, boresight, move(s1)) );
  queue.push( PulseData(1.0, boresight, move(s2)) );
  queue.push( PulseData(1.0, boresight, move(s3)) );  

  assert( !queue.isEmpty() );
  
  queue.empty();

  assert( queue.isEmpty() );

  queue.pushInitial( PulseData(1.0, boresight, move(s1)) );
  queue.push( PulseData(1.0, boresight, move(s2)) );
  queue.push( PulseData(1.0, boresight, move(s3)) );  

  assert( !queue.isEmpty() );  
}

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
      assert(  double_equal(sim_time - old_time, 0.1, 1e-4) );
    }
    if (slow_pop)
      slow_function();
  }
  on = false;
  push_thread.join();
}


void test_empty_queue_exception() {
  math_vector boresight = {1, 0, 0};

  registry s1 = {1};
  registry s2 = {1, 2};

  unsigned short * ptr1 = s1.data();
  unsigned short * ptr2 = s2.data();

  RadarDataQueue queue;

  queue.pushInitial( PulseData(1.0, boresight, move(s1)) ); 
  queue.push( PulseData(1.0, boresight, move(s2)) );
  queue.size();
  queue.pop();
  queue.size();
  queue.pop();
}


void test_pushed_initial_twice() {
  math_vector boresight = {1, 0, 0};

  registry s1 = {1};
  registry s2 = {1, 2};

  unsigned short * ptr1 = s1.data();
  unsigned short * ptr2 = s2.data();

  RadarDataQueue queue;

  queue.pushInitial( PulseData(1.0, boresight, move(s1)) ); 
  queue.pushInitial( PulseData(1.0, boresight, move(s2)) ); 
}


void test_not_pushed_initial() {
  math_vector boresight = {1, 0, 0};

  registry s1 = {1};
  registry s2 = {1, 2};

  unsigned short * ptr1 = s1.data();
  unsigned short * ptr2 = s2.data();

  RadarDataQueue queue;

  queue.push( PulseData(1.0, boresight, move(s1)) ); 
}


int main() {

  test_queue();
  test_empty();
  test_concurrence(false, false);
  test_concurrence(false, true);
  test_concurrence(true, false);
  test_concurrence(true, true);
  assert_throw<logic_error>(&test_empty_queue_exception);
  assert_throw<logic_error>(&test_pushed_initial_twice);
  assert_throw<logic_error>(&test_not_pushed_initial);
  return 0;
}
