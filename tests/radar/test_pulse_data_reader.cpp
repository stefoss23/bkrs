#include <radsim/utils/assert.hpp>

#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/pulse_data_writer.hpp>
#include <radsim/radar/pulse_data_reader.hpp>

using namespace std;
using namespace radsim;

void createDataFile(const string& filename) {
  
  double t1 = 4.5;
  double t2 = 32.25;
  double t3 = 6.5;

  math_vector v = {1.5, 1.25, 1.75};
  vector<unsigned short> data = {3, 300, 700};

  PulseData pulse1(t1, v, data);
  PulseData pulse2(t2, v, data);
  PulseData pulse3(t3, v, data);
  
  PulseDataWriter writer(filename);

  writer.write(pulse1);
  writer.write(pulse2);
  writer.write(pulse3);
  writer.close();
}


void test_reader() {
  const string filename = "filename";

  createDataFile(filename);
  PulseDataReader reader(filename);

  //test read first pulse:
  PulseData pulse1 = reader.read();
  assertFalse(reader.eof());
  assertDoubleEqual(pulse1.getStartTime(), 4.5, 1e-5);
  math_vector boresight = pulse1.getBoresight();

  assertDoubleEqual(boresight[0], 1.5, 1e-5);
  assertDoubleEqual(boresight[1], 1.25, 1e-5);
  assertDoubleEqual(boresight[2], 1.75, 1e-5);

  assertIntEqual(pulse1.registry.size(), 3);
  assertIntEqual(pulse1.registry[0], 3);
  assertIntEqual(pulse1.registry[1], 300);
  assertIntEqual(pulse1.registry[2], 700);
  
  reader.read();
  assertFalse(reader.eof());
  
  PulseData pulse3 = reader.read();  
  assertDoubleEqual(pulse3.getStartTime(), 6.5, 1e-5);
  assertIntEqual(pulse3.registry[2], 700);

  assertTrue(reader.eof());
}


int main() {
  test_reader();
  return 0;
}
