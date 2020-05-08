#ifndef RADAR_RADAR_HPP
#define RADAR_RADAR_HPP

#include <iostream>
#include <memory>
#include <list>

#include <radsim/utils/rng.hpp>

#include <radsim/mathematics/approx_function.hpp>
#include <radsim/mathematics/math_vector.hpp>

#include <radsim/radar/target.hpp>
#include <radsim/radar/pulse_data.hpp>
#include <radsim/radar/beam_pattern.hpp>
#include <radsim/radar/adc.hpp>
#include <radsim/radar/bandpass_filter.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_state.hpp>

namespace radsim {

class Radar 
{
  //parameters read from config object - primary parameters:
  double peak_power; //W
  double antennae_gain; //unit, Antennaegain when a target is in boresight
  double carrier_frequency; //hz
  double pulse_width; //s
  double prt; //s
  double bandwidth; //hz
  double noise_figure; //unit
  double duplexer_switch_time; //s, the time between stop of emission and start of signal reception
  double maximum_receive_time; //s, time upper lim for signal measurement after emission start
  double horizontal_beamwidth; //rad
  double elevation_beamwidth; //rad
  double ant_rot_speed; //rad/s
  double init_hor_theta; //rad, pi/2 - azimuth, horizontal pointing direction

  //Derived parameters. These are parameters that are calculated based on the primary parameters
  double carrier_wavelength; //m
  double instrumented_range; //m
  double unambiguous_range; //m
  double sampling_time; //s, time betweeen samplings
  double avg_noise; //W, the average noise from the receiver
  double minimum_range; //m
  double range_bin; //m
  int    num_range_bins; //m
  double minimum_receive_time; //s, shortest time after emission in which
                             //a signal can be received.
  ADC    adc;                //The analog-to-digital converter used in the radar.
  ComplexApproxFunction bandpass_filter; //func(hz) = unit, Filter used to filter an incoming pulse before sampling.
  DoubleApproxFunction emitted_pulse; //func(s) = unit, shape of emitted pulse
  DoubleApproxFunction filtered_pulse;  //func(s) = unit, Filtered returnpulse reflected from a sphere or point
  DoubleApproxFunction& sim_pulse; //func(s) = unit, the pulse shape used in signal calculations
  DoubleApproxFunction horizontal_beam_shape; //func(rad) = unit
  DoubleApproxFunction elevation_beam_shape; //func(rad) = unit

  //Simulation adjustment parameters
  bool to_add_noise; //if true: noise is added to the total signal calculation
  bool to_add_clutter; //if true: clutter is added to the total signal calculation
  bool to_add_target; //if true: target reflections are added to the total signal calculation
  bool use_pdf; //if true: uses pdf functions, if false:  all use of probability density 
               //functions are shut off, returning mean signal instead
  bool to_use_filtered_pulse; //if_true, then bandpass filtered pulse is used, else incoming. 
  double max_sim_distance; //m, no simulation beyond this distance for either clutter, targets, noise nor civilian jamming. 
  double max_sim_receive_time; //s, corresponding to MaxSimDistance
  
  RadarState state; //contains values that change for each pulse emission cycle. 

  mutable RNG rng;

  void setDerivedParameters();
  void setAvgNoise();
  void setRangeBins(); 
  void setFilteredPulse();

  double  radarEquationPower(double distance, double cross_Section) const; //W, Radar Equation for received power
  //distance: m
  //cross_section: m2

  int     findRangeBin(double ReceiveTime) const; //Finding the rangebin in which a signal (left unfiltered) is first received
  double  getTargetReceiveTime(double distance) const; //s, when the pulse echo from the target is received.
  //distance: m

  //unit, antenna offset modululation for a target
  //1 if in boresight
  double offsetGain(const math_vector& pos) const;

  //Sets the target signal contribution to each range bin position
  //Technical document: Signal Reception / Signal Strength at Sampling Stage
  void    setTargetSignal(std::vector<double>& TargetSignal_I, std::vector<double>& TargetSignal_Q, double ReceiveTime, double SignalPower) const;
          //TargetSignal: amp
          //ReceiveTime: s
          //SignalPower: W
   
  //Returns a sample of the background white noise from the receiver
  double noise(double Q) const; //W
  //Q: [0, 1>, input to PDF from random number generator

  public:
    Radar(const RadarConfig& Config);
    ~Radar();    

    double    getMinimumRange() const; //m
    double    getInstrumentedRange() const; //m
    double    getUnAmbiguousRange() const; //m
    double    getRangeBin() const; //m
    double    getPeakPower() const; //W
    double    getAvgNoise() const; //W
    double    getPulseWidth() const; //s
    double    getPRT() const; //s
    double    getDuplexerSwitchTime() const; //s
    double    getSamplingTime() const; //s, time betweeen samplings, current model uses a fixed formulae
    double    getMaximumReiceiveTime() const; //s
    double    getMinimumReceiveTime() const; //s
    bool      getToAddNoise() const;
    bool      getToAddClutter() const;
    bool      getToAddTarget() const;
    bool      getUsePdf() const;
    bool      getToUseFilteredPulse() const;
    double    getAntRotSpeed() const; //rad/s
    double    getInitialHorTheta() const; //rad, gets the initial horizontal position of the
                                          //     antenna after reset or before any
                                          //     pulse generation

    double    getRange(int bin_index) const; //m, the sampling range corresponding to bin_index. 

    ADC getADC() const;
    DoubleApproxFunction getFilteredPulse() const; //func(s) = unit, on amp level
    DoubleApproxFunction getHorizontalBeamShape() const; //func(rad) = unit, on power level
    DoubleApproxFunction getElevationBeamShape() const; //func(rad) = unit, on power level

    void setAntRotSpeed(double w);
    //w: rad/s

    void setToAddNoise(bool set);
    void setToAddClutter(bool set);
    void setToAddTarget(bool set);
    void setUsePdf(bool set);
    void setToUseFilteredPulse(bool set);

    void setRandomParameters(bool custom, int seed_value, double (*Q_custom)(unsigned int *));
    //if custom: custom seed and chaos functions can be used
    //Q_custom: the custom chaos function. 

    void setInitialHorTheta(double theta_arg); 
    //theta_arg: rad

    const math_vector& getCurrentBoresight() const;
    double getCurrentTime() const; //s
    size_t getCurrentCarrySize() const; //s

    //In addition to generating a PulseData object, this functions changes the state of the radai simulation,
    //with regards to time, antennaeposition, and storing of signals beyong unambiuous range.     
    PulseData generatePulseData(const TargetCollection& targets = {}, bool signal_override = false, double signal_strength = 0);

    void reset(double t = 0);
    //t: s
};

}

#endif


