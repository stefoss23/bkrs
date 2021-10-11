#include <vector>
#include <memory>

#include <radsim/utils/assert.hpp>

#include <radsim/mathematics/math_vector.hpp>

#include <radsim/radar/pulse_data.hpp>

using namespace std;
using namespace radsim;


void test_simple() {
  vector<unsigned short> reg = {1, 2, 3};
  unsigned short * ptr = reg.data();
  double t = 0.25;
  math_vector boresight = {0.1, 0.2, 0.3};

  PulseData data (t, boresight, move(reg) );
  assertTrue( data.isOriginal() );
  assertTrue( data.hasOriginalRegistry() );
  assertTrue( data.getStartTime() == t );
  assertTrue( data.getBoresight() == boresight );
  assertTrue( data.registry.data() == ptr );
}

PulseData generateData(PulseData ** data_ptr, unsigned short ** ptr) {
  vector<unsigned short> reg = {1, 2, 3};
  *ptr = reg.data();
  double t = 0.25;
  math_vector boresight = {0.1, 0.2, 0.3};

  PulseData data(t, boresight, move(reg) );  
  *data_ptr = &data;
  return data;
}

void test_return() {
  unsigned short * ptr;
  PulseData * data_ptr;
  PulseData data = generateData(&data_ptr, &ptr);
  assertTrue( data_ptr == &data);
  assertTrue( ptr == data.registry.data() );
}

void test_copy() {
  PulseData data( 0.0, (math_vector){1, 0, 0}, (vector<unsigned short>){1, 2, 3});
  PulseData copy = data;
  assertFalse(copy.isOriginal());
  assertFalse(copy.hasOriginalRegistry());
}

void test_move() {
  PulseData data( 0.0, (math_vector){1, 0, 0}, (vector<unsigned short>){1, 2, 3});
  PulseData other = move(data);
  assertTrue( other.hasOriginalRegistry() );  
}

int main() {

  test_simple();
  test_return();
  test_copy();
  test_move();
  return 0;
}
