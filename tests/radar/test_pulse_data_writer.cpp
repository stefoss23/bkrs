#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <radsim/utils/assert.hpp>

#include <radsim/mathematics/math_vector.hpp>

#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/pulse_data_writer.hpp>

using namespace radsim;
using namespace std;

int getInt(ifstream& in) {
  int number;
  in.read(reinterpret_cast<char *>(&number), sizeof(number));
  return number;
}

double getDouble(ifstream& in) {
  double number;
  in.read(reinterpret_cast<char *>(&number), sizeof(number));
  return number;
}

double getUShort(ifstream& in) {
  unsigned short number;
  in.read(reinterpret_cast<char *>(&number), sizeof(number));
  return number;
}


void test_write_pulses() {
  const string filename = "filename";

  double t1 = 10.5;
  double t2 = 16.25;

  math_vector v = {0.5, 0.25, 0.75};
  vector<unsigned short> data = {5, 100, 500};

  PulseData pulse1(t1, v, data);
  PulseData pulse2(t2, v, data);
  
  PulseDataWriter writer(filename);

  writer.write(pulse1);
  writer.write(pulse2);
  writer.close();

  ifstream in(filename);
  if (!in)
    throw invalid_argument("File does not exist.");

  //test file version
  assertIntEqual(getInt(in), 0);

  //assertTime
  assertDoubleEqual(getDouble(in), t1, 1e-3);
  //assertDirection
  assertDoubleEqual(getDouble(in), v[0], 1e-3);
  assertDoubleEqual(getDouble(in), v[1], 1e-3);
  assertDoubleEqual(getDouble(in), v[2], 1e-3);
  //assert number of range bins
  assertIntEqual(getInt(in), 3);
  //assert signal data
  assertIntEqual(getUShort(in), data[0]);
  assertIntEqual(getUShort(in), data[1]);
  assertIntEqual(getUShort(in), data[2]);

  //assertTime
  assertDoubleEqual(getDouble(in), t2, 1e-3);
  //assertDirection
  assertDoubleEqual(getDouble(in), v[0], 1e-3);
  assertDoubleEqual(getDouble(in), v[1], 1e-3);
  assertDoubleEqual(getDouble(in), v[2], 1e-3);
  //assert number of range bins
  assertIntEqual(getInt(in), 3);
  //assert signal data
  assertIntEqual(getUShort(in), data[0]);
  assertIntEqual(getUShort(in), data[1]);
  assertIntEqual(getUShort(in), data[2]);  

  in.close();
}


void test_cannot_write_after_closed() {
  PulseDataWriter writer("filename1");
  writer.close();

  math_vector v = {0.5, 0.25, 0.75};
  vector<unsigned short> data = {5, 100, 500};

  PulseData pulse(0.5, v, data);

  assertThrow( writer.write(pulse), logic_error);
}

int main() {
  test_write_pulses();
  test_cannot_write_after_closed();
  return 0;
}
