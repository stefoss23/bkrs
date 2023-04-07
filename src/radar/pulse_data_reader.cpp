#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/pulse_data_reader.hpp>

using namespace radsim;
using namespace std;

PulseDataReader::PulseDataReader(const std::string filename) {
}


PulseData PulseDataReader::read() {
  //return NULL;
}


bool PulseDataReader::eof() {
  return false;
}


void PulseDataReader::close() {
}
