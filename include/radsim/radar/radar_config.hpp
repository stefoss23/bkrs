#include <memory>

#ifndef RADAR_RADAR_CONFIG_H
#define RADAR_RADAR_CONFIG_H

#include <radsim/radar/beam_pattern.hpp>
#include <radsim/radar/adc.hpp>

namespace radsim {

//This is a container for (non-derived) radar parameters. 
//It is used as input for radar object initiation. 
class RadarConfig {
  double  peak_power = 0; //W
  double  frequency = 0; //hz, frequency of emitted signal
  double  pulse_width = 0; //s
  double  sampling_time = 0; //s, inv of sampling ratee of analog converter
  double  prt = 0; //s
  double  maximum_receive_time = 0; //s
  double  bandwidth = 0; //hz
  double  noise_figure = 0; //unit
  double  duplexer_switch_time = 0; //unit, NOT db
  double  horizontal_beamwidth = 0; //rad, NOT degrees
  double  elevation_beamwidth = 0; //rad, NOT degrees
  double  antennae_gain = 0; //unit, NOT db
  double  theta; //rad, pi/2 - azimuth

  double          antennae_rotation_speed; //rad/s, clockwise is positive
  BeamPattern     horizontal_beam_shape;
  BeamPattern     elevation_beam_shape;

  ADCMode      adc_converter_mode;
  int          adc_resolution; //bit
  double       adc_max_power;  //W
  double       adc_min_power;  //W
  double       adc_max2noise; //unit, any value > 0 sets the ADC maxPower relative to radar receiver noise level (in terms of power)
                             //The ADC max level = ADCMaxNoise * AverageInternalRadarNoise
  double       adc_min2noise; //unit, same as for ADCMax2Noise, but sets the minimum measurable power lever. 
  

  bool peak_power_set = false;
  bool frequency_set = false;
  bool pulse_width_set = false;
  bool sampling_time_set = false;
  bool prt_set = false;
  bool maximum_receive_time_set = false;
  bool bandwidth_set = false;
  bool noise_figure_set = false;
  bool duplexer_switch_time_set = false;
  bool hor_beamwidth_set = false;
  bool el_beamwidth_set = false;
  bool antennae_gain_set = false;
  bool theta_set = false;

  public:
    RadarConfig();

    void assertParametersSet() const;

    void setPeakPower(double power);
    //Power: W

    void setFrequency(double f);
    //f: Ghz

    void setPulseWidth(double pulsewidth);
    //pulsewidth: microsec

    void setSamplingTime(double samplingtime);
    //sampligtime: microsec

    void setPRT(double t);
    //t: ms

    void setMaximumReceiveTime(double t);
    //t: ms

    void setBandWidth(double bandWidth);
    //bandWidth: Mhz

    void setNoiseFigure(double noiseFigure);
    //noiseFigure: db, since this is regarded as more front-end.
    
    void setDuplexerSwitchTime(double switchTime);
    //switchTime: microsec   

    void setAntennaeGain(double gain);
    //gain: dB

    void setAzBeamWidth(double horizontalBeamWidth);
    //horizontalBeamWidth: //degrees, NOT rad

    void setElBeamWidth(double elevationBeamWidth);
    //horizontalBeamWidth: //degrees, NOT rad

    void setAntRotSpeed(double speed);
    //speed: degrees/s

    void setAzimuth(double azimuth);
    //azimuth: deg

    void setHorizontalBeamShape(BeamPattern pattern) {
      horizontal_beam_shape = pattern;
    }

    void setElevationBeamShape(BeamPattern pattern) {
      elevation_beam_shape = pattern;
    }

    void setADCMode(ADCMode adcMode);
    void setADCResolution(int resolution);
    //resolution: bit

    void setADCMax2Noise(double ratio);
    //ratio: unit

    void setADCMin2Noise(double ratio);
    //ratio: unit

    double getPeakPower() const; //W
    double getFrequency() const; //hz
    double getPulseWidth() const; //s
    double getSamplingTime() const; //s
    double getPRT() const; //s
    double getMaximumReceiveTime() const; //s
    double getBandWidth() const; //hz
    double getNoiseFigure() const; //unit, NOT db
    double getDuplexerSwitchTime() const; //s
    double getAntennaeGain() const; //unit
    double getHorizontalBeamWidth() const; // rad
    double getElevationBeamWidth() const; // rad
    double getAntRotSpeed() const; // rad/s
    double getTheta() const; // rad

    BeamPattern     getHorizontalBeamShape() const;
    BeamPattern     getElevationBeamShape() const;
    ADCMode         getADCMode() const;
    int             getADCResolution() const; //bit
    double          getADCMax2Noise() const; //unit
    double          getADCMin2Noise() const; //unit
    
};

}

#endif
