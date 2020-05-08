#ifndef RADAR_BANDPASS_FILTER_HPP
#define RADAR_BANDPASS_FILTER_HPP

#include <radsim/mathematics/approx_function.hpp>

namespace radsim {

enum class BandpassFilter{Standard, Rectangular};

ComplexApproxFunction createBandpassFilter(BandpassFilter type, double bandwidth); //func(hz) = unit
//bandwidth: hz

}

#endif
