#include <math.h>

#include <iostream>
#include <exception>
#include <string>
#include <complex>
#include <memory>

#include <radsim/mathematics/constants.hpp>
#include <radsim/mathematics/mathutils.hpp>
#include <radsim/mathematics/approx_function.hpp>
#include <radsim/mathematics/fourier.hpp>
#include <radsim/mathematics/math_vector.hpp>

#include <radsim/radar/radar.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/beam_pattern.hpp>


using namespace std;
using namespace std::complex_literals;


namespace radsim {

Radar::Radar(const RadarConfig& config) :
  adc(1, ADCMode::Power, 1),
  bandpass_filter(0.0),
  horizontal_beam_shape(0.0),
  elevation_beam_shape(0.0),
  emitted_pulse(0.0),
  filtered_pulse(0.0),
  sim_pulse(filtered_pulse),
  state(0, 0)
{
  try {
    config.assertParametersSet();
  }
  catch (logic_error e) {
    cout << e.what() << endl;
    throw invalid_argument(__PRETTY_FUNCTION__ + string(": Logical inconsistencies in RadarConfig argument.") );
  }

  //parameters read from config object - primary parameters:
  peak_power = config.getPeakPower(); //W
  antennae_gain = config.getAntennaeGain(); //unit
  carrier_frequency = config.getFrequency(); //hz
  pulse_width = config.getPulseWidth(); //s
  prt = config.getPRT(); //s
  sampling_time = config.getSamplingTime(); //s
  duplexer_switch_time = config.getDuplexerSwitchTime(); //s
  bandwidth = config.getBandWidth(); //hz
  noise_figure = config.getNoiseFigure(); //unit, NOT is db, because this is back-end!
  maximum_receive_time = config.getMaximumReceiveTime(); //s
  horizontal_beamwidth = config.getHorizontalBeamWidth(); //rad
  elevation_beamwidth = config.getElevationBeamWidth(); //rad
  horizontal_beam_shape = createBeamPattern(config.getHorizontalBeamShape(), horizontal_beamwidth);
  elevation_beam_shape = createBeamPattern(config.getElevationBeamShape(), elevation_beamwidth);
  ant_rot_speed = config.getAntRotSpeed(); //rad/s
  init_hor_theta = config.getTheta(); //rad

  setDerivedParameters();

  try {
    adc = ADC(config.getADCResolution(),
              config.getADCMode(),
              config.getADCMin2Noise() * avg_noise,
              config.getADCMax2Noise() * avg_noise);
  }
  catch (invalid_argument e) {
    cout << e.what() << endl;
    throw invalid_argument(__PRETTY_FUNCTION__ + string(": Error loading ADC from radar_config parameters."));
  }

  //Simulation parameters
  to_add_noise = true;
  to_add_clutter = true;
  to_add_target = true;
  use_pdf = true; 
  to_use_filtered_pulse = true;
  max_sim_distance = 150000; //m
  max_sim_receive_time = (2 * max_sim_distance) / speed_of_light;
}

Radar::~Radar()
{
}

//These are parameters that are calculated based on the primary parameters
void Radar::setDerivedParameters() {
  state.reset(0, init_hor_theta);
  bandpass_filter = createBandpassFilter(BandpassFilter::Standard, bandwidth); //f(hz) = unit
  carrier_wavelength = speed_of_light / carrier_frequency; //m
  instrumented_range = speed_of_light * maximum_receive_time / 2.0; //m
  unambiguous_range = speed_of_light * prt / 2.0; //m
  minimum_receive_time = duplexer_switch_time + pulse_width; //s
  minimum_range = speed_of_light * minimum_receive_time / 2.0; //m
  setAvgNoise();
  setRangeBins();
  emitted_pulse = DoubleApproxFunction( {0, pulse_width}, 
                                        (vector<double>){1, 1}, 0, 0);
  setFilteredPulse();
  sim_pulse = filtered_pulse;
}


//See technical documentation: Signal Reception, intro (cheated on the integration part).
void Radar::setAvgNoise() 
{
  double Temperature = 300; // K 
  avg_noise = noise_figure * BoltzmannsConstant * Temperature * bandwidth; //W
}


//Note: the current model uses sampling frequency = 1 / Pulsewidth
void Radar::setRangeBins() 
{
  range_bin = speed_of_light * sampling_time / 2.0; //m
  num_range_bins = (maximum_receive_time - minimum_receive_time) / sampling_time;
}


//Sets the shape of the filtered pulse, s_0, based on the shape of the incoming pulse and the bandpass filter. 
//See technical document: Signal Reception / BandPass filtering of target signals
void Radar::setFilteredPulse()
{
  //Set the shape of the incoming pulse, s,  as an approxfunction
  const auto& s = emitted_pulse; //func(s) = unit
    
  //Fourier transforming the function s(t)-> S(f)
  double dt = pulse_width * 1e-3; //s
  int num_frequencies = 1000;
  double f_min = -3.0 / pulse_width; //hz
  double f_max =  3.0 / pulse_width; //hz
  double df = (f_max - f_min) / num_frequencies; //hz
  vector<double> frequency(num_frequencies); //hz
  for (int n = 0; n < num_frequencies; n++) 
    frequency[n] = f_min + df * n; //hz
    
  auto S = fourierTransform(s, frequency, dt, 0.0, pulse_width); //func(hz) = unit
    
  //modulating S with the bandpass filter
  vector<complex<double>> HS(num_frequencies); //hz
  for (int n = 0; n < num_frequencies; n++)
    HS[n] = S.output(frequency[n]) * bandpass_filter.output(frequency[n]);

  ComplexApproxFunction Modulated(frequency, HS); //func(hz) = unit
    
  //Finally, finding the filtered pulse:
  int num_times = 1000;
  vector<double> Time(num_times); //s
  double t_min = -2.0 * pulse_width; //s
  double t_max = 3.0 * pulse_width; //s
  dt = (t_max - t_min) / num_times; //s
  for (int n = 0; n < num_times; n++)
    Time[n] = t_min + dt * n; //s

  auto filtered_pulse_complex = fourierTransform(Modulated, Time, df, f_min, f_max, true); //func(s) = unit
  vector<double> value(num_times);
  for (int n = 0; n < num_times; n++) {
    value[n] = abs( filtered_pulse_complex.getValueVector()[n] ); //unit
  }
  filtered_pulse = DoubleApproxFunction(move(Time), value); //func(s) = unit
} 

// unit f(hz)
DoubleApproxFunction Radar::getFilteredPulse() const
{
   return filtered_pulse; //unit f(hz)
}

//func(rad) = unit, on power level
DoubleApproxFunction Radar::getHorizontalBeamShape() const {
  return horizontal_beam_shape;
}

//func(rad) = unit, on power level
DoubleApproxFunction Radar::getElevationBeamShape() const {
  return elevation_beam_shape;
}

//m
double Radar::getMinimumRange() const {
  return minimum_range; //m
}

//m
double Radar::getUnAmbiguousRange() const
{
  return unambiguous_range; //m
}

//m
double Radar::getInstrumentedRange() const
{
  cout << "WARNING: Instrumented range is deprecated." << endl;
  return instrumented_range; //m
}

//m
double Radar::getRangeBin() const
{
  return range_bin;
}

//W
double Radar::getPeakPower() const
{
  return peak_power; //W
}

//s
double Radar::getSamplingTime() const
{
   return sampling_time; //s
}

//s
double Radar::getMaximumReiceiveTime() const
{
  return maximum_receive_time;
}

//s
double Radar::getMinimumReceiveTime() const
{
   return minimum_receive_time;
}

bool Radar::getToAddNoise() const
{
   return to_add_noise;
}

bool Radar::getToAddClutter() const
{
   return to_add_clutter;
}

bool Radar::getToAddTarget() const
{
   return to_add_target;
}

bool Radar::getUsePdf() const
{
   return use_pdf;
}

//m
double Radar::getRange(int bin_index) const {
   return minimum_range + bin_index * range_bin; //m
}

void Radar::setToAddNoise(bool set)
{
   to_add_noise = set;
}

void Radar::setToAddClutter(bool set)
{
   to_add_clutter = set;
}

void Radar::setToAddTarget(bool set)
{
   to_add_target = set;
}

void Radar::setUsePdf(bool set) {
   use_pdf = set;
}

void Radar::setToUseFilteredPulse(bool set) {
  to_use_filtered_pulse = set;
  if (set)
    sim_pulse = filtered_pulse;
  else
    sim_pulse = emitted_pulse;
}

//W
double Radar::getAvgNoise() const {
   return avg_noise; //W
}

//s
double Radar::getPulseWidth() const {
   return pulse_width;
}

//s
double Radar::getPRT() const {
  return prt; //s
}

//s
double Radar::getDuplexerSwitchTime() const {
  return duplexer_switch_time; //s
} 

ADC Radar::getADC() const
{
  return adc;
}

//rad/s
double Radar::getAntRotSpeed() const {
  return ant_rot_speed; //rad/s
}


void Radar::setAntRotSpeed(double w)
//w: rad/s
{
  ant_rot_speed = w; //rad/s
}

bool Radar::getToUseFilteredPulse() const {
  return to_use_filtered_pulse;
}

void Radar::setRandomParameters(bool custom, int seed_value, double (*Q_custom)(unsigned int *))
{
  rng = RNG(custom, seed_value, Q_custom);
}

//s, initial horizontal position of antenna
//   after reset or before any pulse generation
double Radar::getInitialHorTheta() const {
  return init_hor_theta; //rad
}

//W
//Returns the background white noise from the receiver
double Radar::noise(double Q) const
//Q: [0, 1>, input to PDF from random number generator
{
   if (use_pdf)
     return rayleighPDF(avg_noise, Q); //W
   else
     return avg_noise;
}

void Radar::setInitialHorTheta(double theta_arg)
//theta_arg: rad
{
  init_hor_theta = theta_arg; //rad
}


//W, Radar Equation for received power
double Radar::radarEquationPower(double distance, double cross_section) const
//distance: m
//cross_section: m2
{
  return peak_power * antennae_gain * antennae_gain * cross_section * carrier_wavelength * carrier_wavelength / 
         ( pow(4 * pi, 3) * pow(distance, 4) );
}

//Finding the rangebin in which a signal (left unfiltered) is first received
//Technical document: Signal Reception / Signal Strength at Sampling Stage
int Radar::findRangeBin(double ReceiveTime) const {
  return (ReceiveTime - minimum_receive_time) / sampling_time; //note: this can be outside current range bin registry.
}


//s
double Radar::getTargetReceiveTime(double distance) const
//distance: m
{
  return (2 * distance / speed_of_light);
}

//unit, antenna offset modululation for a target
//1 if in boresight
double Radar::offsetGain(const math_vector& pos) const {

  double z = pos * state.getBoresight();
  double x = pos * state.getFrameX();
  double y = pos * state.getFrameY();
  double offset_gain = 0;
  if (z > 0) {
    double hor_dev = acos( z / sqrt(x*x + z*z) ); //rad
    double el_dev  = acos( z / sqrt(y*y + z*z) ); //rad
    offset_gain = horizontal_beam_shape.output(hor_dev) * elevation_beam_shape.output(el_dev); //unit
  }
  return offset_gain; //unit
}


void Radar::setTargetSignal(std::vector<double>& TargetSignal_I, std::vector<double>& TargetSignal_Q, double ReceiveTime, double SignalPower) const
//TargetSignals: amp
//ReceiveTime: s
//SignalPower: W
{
  int TargetBin = findRangeBin(ReceiveTime);
  int FirstTargetBin = TargetBin - 3;
  int LastTargetBin  = TargetBin + 4;
  double Value = powerToAmp(SignalPower); //amp
  for (int n = FirstTargetBin; n <= LastTargetBin; n++)
    if (n >= 0 && n < num_range_bins) {
      //FilteredPulse adjusts the incoming signal due to bandpass filtering. 
      double phase = 2 * pi * rng.output(); //rad
      double bin_signal = Value * sim_pulse.output( minimum_receive_time + n * sampling_time - ReceiveTime ); //amp, power per range bin, due to filtering
      TargetSignal_I[n] += bin_signal * cos(phase); //amp
      TargetSignal_Q[n] += bin_signal * sin(phase); //amp
    }
}


//In addition to generating a PulseData object, this functions changes the state of the radai simulation,
//with regards to time, antennaeposition, and storing of signals beyong unambiuous range.     
PulseData Radar::generatePulseData(const TargetCollection& targets, bool signal_override, double signal_strength)
//signal_override: if true, target signal is signal_strength at boresight
//signal_strength: W
{
  //transfer data from State:
  double state_time = state.getTime(); //s, the time when pulse emission begins. 
  auto& list_carry = state.getListCarry();

  //Calculations from target(s)
  vector<double> target_signal_I(num_range_bins, 0); //amp
  vector<double> target_signal_Q(num_range_bins, 0); //amp

  if (to_add_target) {

    //looping over signals reflected from beyong unambiguous range in previous emission period(s).
    auto it = list_carry.begin();
    while (it != list_carry.end()) {
      if (it->time < prt) {
        double offset_gain = offsetGain(it->pos);
        setTargetSignal(target_signal_I, target_signal_Q, it->time, it->power * offset_gain);
        it = list_carry.erase(it);
      }
      else {
        it->time -= prt;
        it++;
      }
    }

    //Then handling new cases:
    for (const Target& target : targets) {
      double rcs = target.getRCS();
      math_vector pos = target.getPosition(state_time);
      double received_boresight_power; //W, received power if target was in boresight
      double target_distance = math_vector_length(pos); //m

      if (signal_override)
        received_boresight_power = signal_strength; //W
      else
        received_boresight_power = radarEquationPower(target_distance, rcs); //W

      double offset_gain = offsetGain(pos); //unit
      double signal_power = offset_gain * received_boresight_power; //W
      double receive_time = getTargetReceiveTime(target_distance); //s

      if (receive_time > prt) {
        if (receive_time > prt && receive_time <= max_sim_receive_time)
          list_carry.emplace_back(receive_time - prt, signal_power, pos);
      }
      else 
        setTargetSignal(target_signal_I, target_signal_Q, receive_time, signal_power * offset_gain);
    }
  }

  //Final Assembly: combination of target and noise
  vector<unsigned short> new_registry(num_range_bins);
  for (int n = 0; n < num_range_bins; n++)
  {
    double noise_amplitude = 0;
    if (to_add_noise)
      noise_amplitude = powerToAmp(noise( rng.output()  )); //amp

    double amp_I = noise_amplitude + target_signal_I[n]; //amp
    double amp_Q = target_signal_Q[n]; //amp
    double amplitude = sqrt( amp_I * amp_I + amp_Q * amp_Q ); //amp
    new_registry[n] = adc.convertSignal(amplitude); //unit
  }

  PulseData pulse_data(state_time, state.getBoresight(), move(new_registry));
  state.incrementParams(prt, prt * ant_rot_speed);
  return pulse_data;
}

void Radar::reset(double t)
//t: s 
{
  state.reset(t, init_hor_theta);
}

const math_vector& Radar::getCurrentBoresight() const {
  return state.getBoresight();
}

size_t Radar::getCurrentCarrySize() const {
  return state.getCarrySize();
}

//s
double Radar::getCurrentTime() const {
  return state.getTime(); //s
}


} //end namespace bkradsim

