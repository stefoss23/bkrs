
#include <vector>
#include <memory>
#include <thread>

#include <radsim/utils/assert.hpp>

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

  assertTrue( queue.isEmpty() );
  assertIntEqual( queue.size(), 0 );
  queue.pushInitial( PulseData(1.0, boresight, move(s1)) );  // ----- queue = {p1}
  assertFalse( queue.isEmpty() );
  assertIntEqual( queue.size(), 1 );
  queue.push( PulseData(2.0, boresight, move(s2)) );  // ----- queue = {p1, p2}
  assertTrue( queue.size() );
  auto pkg_1 = queue.pop(); // ---- queue = {p2}
  assertTrue( pkg_1.registry.data() == ptr1 );
  assertTrue( pkg_1.hasOriginalRegistry() );
  assertIntEqual( queue.size(), 1 );
  assertFalse( queue.isEmpty() );
  queue.push( PulseData(3.0, boresight, move(s3)) ); // ------ queue = {p2, p3}
  assertIntEqual( queue.size(), 2 );
  auto pkg_2 = queue.pop(); // ---- queue = {p3}
  assertTrue( pkg_2.registry.data() == ptr2 );
  assertIntEqual( queue.size(), 1);
  queue.push( PulseData(4.0, boresight, move(s4)) ); // ------ queue = {p3, p4}
  assertFalse( queue.isEmpty() );
  assertIntEqual( queue.size(), 2 );
  queue.push( PulseData(5.0, boresight, move(s5)) ); // ------ queue = {p3, p4, p5}
  assertIntEqual( queue.size(), 3 );
  queue.push( PulseData(6.0, boresight, move(s6)) ); // ------ queue = {p3, p4, p5, p6}
  auto pkg_3 = queue.pop(); // ---- queue = {p4, p5, p6}
  assertTrue( pkg_3.registry.data() == ptr3 );
  assertIntEqual( queue.size(), 3);
  auto pkg_4 = queue.pop(); // ---- queue = {p5, p6}
  assertTrue( pkg_4.registry.data() == ptr4 );
  assertTrue( queue.size() == 2);

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

  assertFalse( queue.isEmpty() );
  
  queue.empty();

  assertTrue( queue.isEmpty() );

  queue.pushInitial( PulseData(1.0, boresight, move(s1)) );
  queue.push( PulseData(1.0, boresight, move(s2)) );
  queue.push( PulseData(1.0, boresight, move(s3)) );  

  assertFalse( queue.isEmpty() );  
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
  assertThrow(test_empty_queue_exception(), logic_error);
  assertThrow(test_pushed_initial_twice(), logic_error);
  assertThrow(test_not_pushed_initial(), logic_error);
  return 0;
}
