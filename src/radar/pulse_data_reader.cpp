#include <vector>
#include <iostream>

#include <radsim/mathematics/math_vector.hpp>

#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/pulse_data_reader.hpp>

using namespace radsim;
using namespace std;

PulseDataReader::PulseDataReader(const std::string filename) :
  in(filename)
{
  if (!in)
    throw invalid_argument(__PRETTY_FUNCTION__ + string(": error reading file: '" + filename + "'"));

  //read fileversion
  cout << "version = " << read<int>() << endl;
}


PulseData PulseDataReader::read() {

  double t = read<double>();

  cout << "--- time = " << t << endl;

  double x;
  double y;
  double z;

  return PulseData(t, math_vector {x, y, z}, vector<unsigned short>(0));
}


bool PulseDataReader::eof() {
  return false;
}


void PulseDataReader::close() {
}
