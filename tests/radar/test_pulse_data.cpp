#include <assert.h>

#include <vector>
#include <memory>

#include <radsim/utils/test_utils.hpp>

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
  assert( data.isOriginal() );
  assert( data.hasOriginalRegistry() );
  assert( data.getStartTime() == t );
  assert( data.getBoresight() == boresight );
  assert( data.registry.data() == ptr );
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
  assert( data_ptr == &data);
  assert( ptr == data.registry.data() );
}

void test_copy() {
  PulseData data( 0.0, (math_vector){1, 0, 0}, (vector<unsigned short>){1, 2, 3});
  PulseData copy = data;
  assert(!copy.isOriginal());
  assert(!copy.hasOriginalRegistry());
}

void test_move() {
  PulseData data( 0.0, (math_vector){1, 0, 0}, (vector<unsigned short>){1, 2, 3});
  PulseData other = move(data);
  assert( other.hasOriginalRegistry() );  
}

int main() {

  test_simple();
  test_return();
  test_copy();
  test_move();
  return 0;
}
