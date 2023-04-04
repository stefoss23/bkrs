#include <iostream>

#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/pulse_data_writer.hpp>

using namespace radsim;
using namespace std;

PulseDataWriter::PulseDataWriter(const std::string& filename) :
  ofs(filename)
{
  //write file version
  write<int>(0);
}

void PulseDataWriter::write(const PulseData& pulse_data) {
    
  if (is_closed)
    throw logic_error(__PRETTY_FUNCTION__ + string(": PulseDataWriter cannot write after being closed."));

  write<double>(pulse_data.getStartTime());

  math_vector boresight = pulse_data.getBoresight();
  write<double>(boresight[0]);
  write<double>(boresight[1]);
  write<double>(boresight[2]);

  write<int>(pulse_data.registry.size());
  for (unsigned short i : pulse_data.registry)
    write<unsigned short>(i);
}

void PulseDataWriter::close() {
  is_closed = true;
  ofs.close();
}
