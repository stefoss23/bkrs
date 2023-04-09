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
  read<int>();
}


PulseData PulseDataReader::read() {

  //pulse start time
  double t = read<double>(); //s

  //direction
  double x = read<double>();
  double y = read<double>();
  double z = read<double>();

  int size = read<int>();
  vector<unsigned short> data(size);
  for (int i = 0; i < size; i++)
    data[i] = read<unsigned short>();

  return PulseData(t, math_vector {x, y, z}, data);
}


bool PulseDataReader::eof() {
  return false;
}


void PulseDataReader::close() {
}