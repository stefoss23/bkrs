#include <vector>
#include <array>
#include <complex>
#include <exception>
#include <string>
#include <memory>

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "export.hpp"
#include "convert.hpp"

#include <radsim/mathematics/approx_function.hpp>

#include <radsim/radar/target.hpp>
#include <radsim/radar/adc.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>
#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/radar.hpp>

using namespace std;
using namespace std::complex_literals;
using namespace radsim;
namespace py = pybind11;


namespace {

  py::array_t<double> TargetGetPosition(const Target& T, double t = 0) {
    return py_convert::numpy_array( T.getPosition(t) );
  }

  /*
     Copies data into TargetCollection, instead of referencing. This is due to the differences in memory handling between C++ and Python.
  */
  class PythonTargetCollection {
    private:
      TargetCollection collection;

      TargetCollection::iterator iGet(int indx) {
        if (indx < 0 || indx >= collection.size())
          throw invalid_argument(__PRETTY_FUNCTION__ + string(": invalid index."));

        auto it = collection.begin();
        int count = 0;
        while (count != indx ) {
          it++;
          count++;
        }

        return it;
      }

    public:
      PythonTargetCollection() = default;

      int size() {
        return collection.size();
      }

      void appendCopy(const Target& target) {
        collection.emplace_back(target);
      }

      Target iGetCopy(int indx) {
        return *iGet(indx);        
      }

      void iDelete(int indx) {
        collection.erase( iGet(indx) );
      }

      void clear() {
        collection.clear();
      }

      const TargetCollection& getList() const {
        return collection;
      }
  };

}



PYBIND11_MODULE(radar_python, m) {


  // ************************* Functions ************************************************

  py::enum_<BandpassFilter>(m, "BandpassFilter")
     .value("Standard", BandpassFilter::Standard)
     .value("Rectangular", BandpassFilter::Rectangular)
  .export_values();

  m.def("create_bandpass_filter", &createBandpassFilter);

  
  py::enum_<BeamPattern>(m, "BeamPattern")
     .value("Gaussian", BeamPattern::Gaussian)
  .export_values();

  m.def("create_beam_pattern", &createBeamPattern);
  

  // ************************ Target ************************************************ 
  py::class_<Target> (m, "Target")
  .def(py::init([](VectorApproxFunction path, double rcs) {
     return Target( move(path), rcs);
   }  )  )
  .def(py::init([](py::array_t<double> py_pos, double rcs) {
      math_vector pos = py_convert::math_vector(py_pos);
      return Target(pos, rcs);
   }  )  )
  
  .def("get_position", &TargetGetPosition, py::arg("t") = 0)
  .def_property_readonly("rcs", &Target::getRCS)

  .def("get_path", [](const Target& target) -> VectorApproxFunction { 
    VectorApproxFunction path = target.getPath();
    return path;
   } )

   .def("set_position", [](Target& target, py::array_t<double> py_pos) {
     target.setPosition( py_convert::math_vector(py_pos) );
   } )
   ;



  // ****************** TargetCollection *********************************************
  py::class_<PythonTargetCollection> (m, "TargetCollection")
  .def(py::init<>())
  .def("append_copy", &PythonTargetCollection::appendCopy)
  .def("iget_copy", &PythonTargetCollection::iGetCopy)
  .def("idelete", &PythonTargetCollection::iDelete)
  .def("clear", &PythonTargetCollection::clear)
  .def_property_readonly("size", &PythonTargetCollection::size)
  ;


  // ************************* ADC - Analog to Digital Converter *************************

  py::enum_<ADCMode>(m, "ADCMode")
     .value("Power", ADCMode::Power)
     .value("Logarithm", ADCMode::Logarithm)
  .export_values();

  py::class_<ADC> (m, "ADC")
  .def(py::init([](int resolution, ADCMode mode, double maximumPower, double minimumPower) {
       return ADC(resolution, mode, maximumPower, minimumPower);
     } ), py::arg("numLevels"), py::arg("mode"), py::arg("maximumPower"),  py::arg("minimumPower") = 0  )

   .def_property_readonly("num_levels", &ADC::getNumLevels)
   .def("convert", &ADC::convertSignal);



  //*************************** RadarConfig *************************
  py::class_<RadarConfig> (m, "RadarConfig")
  .def(py::init<>())
  .def("assert_parameters_set", &RadarConfig::assertParametersSet)

  .def_property("frequency", &RadarConfig::getFrequency, &RadarConfig::setFrequency)
  .def_property("peak_power", &RadarConfig::getPeakPower, &RadarConfig::setPeakPower)
  .def_property("pulse_width", &RadarConfig::getPulseWidth, &RadarConfig::setPulseWidth)
  .def_property("sampling_time", &RadarConfig::getSamplingTime, &RadarConfig::setSamplingTime)
  .def_property("prt", &RadarConfig::getPRT, &RadarConfig::setPRT)
  .def_property("bandwidth", &RadarConfig::getBandWidth, &RadarConfig::setBandWidth)
  .def_property("max_receive_time", &RadarConfig::getMaximumReceiveTime, &RadarConfig::setMaximumReceiveTime)
  .def_property("duplex_switch_time", &RadarConfig::getDuplexerSwitchTime, &RadarConfig::setDuplexerSwitchTime)
  .def_property("noise_figure", &RadarConfig::getNoiseFigure, &RadarConfig::setNoiseFigure)
  .def_property("ant_gain", &RadarConfig::getAntennaeGain, &RadarConfig::setAntennaeGain)
  .def_property("az_beamwidth", &RadarConfig::getHorizontalBeamWidth, &RadarConfig::setAzBeamWidth)
  .def_property("el_beamwidth", &RadarConfig::getElevationBeamWidth, &RadarConfig::setElBeamWidth)
  .def_property("ant_rot_speed", &RadarConfig::getAntRotSpeed, &RadarConfig::setAntRotSpeed)
  .def_property("azimuth", &RadarConfig::getTheta, &RadarConfig::setAzimuth)
  .def_property("adc_resolution", &RadarConfig::getADCResolution, &RadarConfig::setADCResolution)
  .def_property("adc_mode", &RadarConfig::getADCMode, &RadarConfig::setADCMode)
  .def_property("adc_min_2_noise", &RadarConfig::getADCMin2Noise, &RadarConfig::setADCMin2Noise)
  .def_property("adc_max_2_noise", &RadarConfig::getADCMax2Noise, &RadarConfig::setADCMax2Noise)
  ;


  // ************************** RadarConfigParser *******************
  py::class_<RadarConfigParser> (m, "RadarConfigParser")
  .def(py::init<>())
  .def("parse_file", &RadarConfigParser::parseFile)
  .def("parse_string", &RadarConfigParser::parseString)
  ;


  // *********************** PulseData ********************
  py::class_<PulseData> (m, "PulseData")

  .def("get_copy_registry", [](PulseData& data) -> py::array_t<unsigned short> { 
    return py_convert::numpy_array( data.registry );
   } )

  .def_property_readonly("boresight", [](PulseData& data) -> py::array_t<double> { 
      return py_convert::numpy_array( data.getBoresight() );
    } )

  .def("__len__", [](PulseData& data) -> int { 
      return data.registry.size();
    } )

  .def("has_original", &PulseData::hasOriginalRegistry)
  .def_property_readonly("time", &PulseData::getStartTime)
  ;


  // ************************* Radar *****************************
  py::class_<Radar> (m, "Radar")
  .def(py::init<const RadarConfig&>())

  .def("generate_pulse_data", [](Radar& radar) -> PulseData { 
      return radar.generatePulseData();
    } )

  .def("generate_pulse_data", [](Radar& radar, const PythonTargetCollection& collection, bool signal_override, double signal_strength) -> PulseData { 
      return radar.generatePulseData(collection.getList(), signal_override, signal_strength);
    }, py::arg("collection"), py::arg("signal_override") = false, py::arg("signal_strength") = 0 )

  .def("reset", [](Radar& radar, double t) { 
      radar.reset(t);
    }, py::arg("t") = 0 )

  .def("get_current_boresight", [](Radar& radar) -> py::array_t<double> {
      return py_convert::numpy_array( radar.getCurrentBoresight() );
    })

  .def("get_filtered_pulse", &Radar::getFilteredPulse)
  .def("get_range", &Radar::getRange)
  .def("get_adc", &Radar::getADC)
  .def("get_horizontal_beam_shape", &Radar::getHorizontalBeamShape)
  .def("get_elevation_beam_shape", &Radar::getElevationBeamShape)

  .def_property_readonly("peak_power", &Radar::getPeakPower)
  .def_property_readonly("pulse_width", &Radar::getPulseWidth)
  .def_property_readonly("prt", &Radar::getPRT)
  .def_property_readonly("sampling_time", &Radar::getSamplingTime, "s, time betweeen samplings, current model uses a fixed formulae")
  .def_property_readonly("duplexer_switch_time", &Radar::getDuplexerSwitchTime)
  .def_property_readonly("max_receive_time", &Radar::getMaximumReiceiveTime)
  .def_property_readonly("minimum_range", &Radar::getMinimumRange)
  .def_property_readonly("instrumented_range", &Radar::getInstrumentedRange)
  .def_property_readonly("unambiguous_range", &Radar::getUnAmbiguousRange)
  .def_property_readonly("range_bin", &Radar::getRangeBin)
  .def_property_readonly("avg_noise", &Radar::getAvgNoise)
  .def_property_readonly("minimum_receive_time", &Radar::getMinimumReceiveTime, "s, time between start of pulse emission and first signal sampling.")
  .def_property_readonly("current_time", &Radar::getCurrentTime)

  .def_property("add_noise", &Radar::getToAddNoise, &Radar::setToAddNoise)
  .def_property("add_target", &Radar::getToAddTarget, &Radar::setToAddTarget)
  .def_property("use_pdf", &Radar::getUsePdf, &Radar::setUsePdf)
  .def_property("use_filtered_pulse", &Radar::getToUseFilteredPulse, &Radar::setToUseFilteredPulse)
  .def_property("ant_rot_speed", &Radar::getAntRotSpeed, &Radar::setAntRotSpeed)
  .def_property("initial_hor_theta", &Radar::getInitialHorTheta, &Radar::setInitialHorTheta)

  .def("get_current_theta", &Radar::getCurrentHorTheta)
  ;

}
