#include <iostream>

#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/pulse_data_writer.hpp>

using namespace radsim;
using namespace std;

PulseDataWriter::PulseDataWriter(const std::string& filename) :
  ofs(filename)
{
  //write file version
  writeInt(0);
}

void PulseDataWriter::write(const PulseData& pulse_data) {
    
  if (is_closed)
    throw logic_error(__PRETTY_FUNCTION__ + string(": PulseDataWriter cannot write after being closed."));

  writeDouble(pulse_data.getStartTime());

  math_vector boresight = pulse_data.getBoresight();
  writeDouble(boresight[0]);
  writeDouble(boresight[1]);
  writeDouble(boresight[2]);

  writeInt(pulse_data.registry.size());
  for (unsigned short i : pulse_data.registry)
    writeUnsignedShort(i);
}

void PulseDataWriter::close() {
  is_closed = true;
  ofs.close();
}

void PulseDataWriter::writeDouble(double number) {
  ofs.write((char *) &number, sizeof number);
}

void PulseDataWriter::writeInt(int number) {
  ofs.write((char *) &number, sizeof number);
}

void PulseDataWriter::writeUnsignedShort(unsigned short number) {
  ofs.write((char *) &number, sizeof number);
}
