#include <memory>

#ifndef RADAR_BEAM_SHAPE_HPP
#define RADAR_BEAM_SHAPE_HPP

#include <radsim/mathematics/approx_function.hpp>

namespace radsim {

enum class BeamPattern{Triangular, Gaussian};

DoubleApproxFunction createBeamPattern(BeamPattern Shape, double beamwidth); //func(rad)=unit
//beamwidth: rad

}

#endif
