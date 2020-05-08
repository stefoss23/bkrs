#include <radsim/radar/pulse_data.hpp>

using namespace std;

namespace radsim {

PulseData::PulseData(double t, math_vector boresight_arg, vector<unsigned short> registry_arg) {
  origin_reg = registry_arg.data();
  t_start = t;
  boresight = boresight_arg,
  registry = move(registry_arg),
  origin_data = this;
}

bool PulseData::isOriginal() const {
  return (origin_data == this);
}

bool PulseData::hasOriginalRegistry() const {
  return (origin_reg == registry.data());
}

double PulseData::getStartTime() const {
  return t_start;
}

math_vector PulseData::getBoresight() const {
  return boresight;
}

}
