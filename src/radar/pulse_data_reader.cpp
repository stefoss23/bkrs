#include <vector>
#include <iostream>

#include <radsim/mathematics/math_vector.hpp>

#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/pulse_data_reader.hpp>

using namespace radsim;
using namespace std;

PulseDataReader::PulseDataReader(const std::string filename) :
  in(filename),
  is_closed(false)
{
  if (!in)
    throw invalid_argument(__PRETTY_FUNCTION__ + string(": error reading file: '" + filename + "'"));

  //read fileversion
  read<int>();
}


PulseData PulseDataReader::read() {

  if (is_closed)
    throw logic_error(__PRETTY_FUNCTION__ + string(": cannot read when reader is closed."));

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
  in.peek();
  return in.eof();
}

void PulseDataReader::assertNotEndOfFile() {
  if (in.eof())
    throw logic_error(__PRETTY_FUNCTION__ + string(": Reached unexpected end-of-file."));
}

void PulseDataReader::close() {
  in.close();
  is_closed = true;
}
