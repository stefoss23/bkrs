#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <vector>
#include <algorithm>

#include <radsim/radar/radar_config_parser.hpp>

using namespace std;
using namespace radsim;

namespace {

  //Keyword Definitions
  const string FREQUENCY = "Frequency";
  const string PEAK_POWER = "PeakPower";
  const string PULSEWIDTH = "PulseWidth";
  const string ANTROTSPEED = "AntRotationSpeed";
  const string SAMPLINGTIME = "SamplingTime";
  const string PRT = "PRT";
  const string MAXRECEIVETIME = "MaxReceiveTime";
  const string NOISE_FIGURE = "NoiseFigure";
  const string DUPLEXSWITCHTIME = "DuplexSwitchTime";
  const string ANTENNAEGAIN = "AntennaeGain";
  const string BANDWIDTH = "BandWidth";
  const string AZBEAMWIDTH = "AzBeamWidth";
  const string ELBEAMWIDTH = "ElBeamWidth";
  const string AZIMUTH = "Azimuth";
  const string ADCRESOLUTION = "ADCResolution";
  const string ADCMODE = "ADCMode";
  const string ADCMIN2NOISE = "ADCMin2Noise";
  const string ADCMAX2NOISE = "ADCMax2Noise";

  //map between keyword and RadarConfigType
  map<string, RadarConfigType> createKeyTypes() {
    map<string, RadarConfigType> type_dict = {
      {FREQUENCY,        RadarConfigType::fdouble},
      {PEAK_POWER,       RadarConfigType::fdouble},
      {PULSEWIDTH,       RadarConfigType::fdouble},
      {ANTROTSPEED,      RadarConfigType::fdouble},
      {MAXRECEIVETIME,   RadarConfigType::fdouble},
      {NOISE_FIGURE,     RadarConfigType::fdouble},
      {SAMPLINGTIME,     RadarConfigType::fdouble},
      {PRT,              RadarConfigType::fdouble},     
      {DUPLEXSWITCHTIME, RadarConfigType::fdouble},
      {ANTENNAEGAIN,     RadarConfigType::fdouble},
      {BANDWIDTH,        RadarConfigType::fdouble},
      {AZBEAMWIDTH,      RadarConfigType::fdouble},
      {ELBEAMWIDTH,      RadarConfigType::fdouble},
      {AZIMUTH,          RadarConfigType::fdouble},
      {ADCRESOLUTION,    RadarConfigType::integer},
      {ADCMODE,          RadarConfigType::fstring},
      {ADCMIN2NOISE,     RadarConfigType::fdouble},
      {ADCMAX2NOISE,     RadarConfigType::fdouble}
      };
    return type_dict;
  }

  bool is_int(const std::string& s)
  {
    return !s.empty() && std::find_if(s.begin(),
      s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
  }

  vector<string> splitLine(const string& line) {
    string buf;  
    stringstream ss(line);
    vector<string> tokens;

    while (ss >> buf && buf[0] != '#')
       tokens.push_back(buf);

    return tokens;
  }

}

namespace radsim {


void RadarConfigParser::setKeyordParams(RadarConfig& config, std::string keyword, RadarConfigValue value) const {
  if (keyword == FREQUENCY)             config.setFrequency(value.dval);
  else if (keyword == PEAK_POWER)       config.setPeakPower(value.dval);
  else if (keyword == PULSEWIDTH)       config.setPulseWidth(value.dval);
  else if (keyword == ANTROTSPEED)      config.setAntRotSpeed(value.dval);
  else if (keyword == SAMPLINGTIME)     config.setSamplingTime(value.dval);
  else if (keyword == PRT)              config.setPRT(value.dval);
  else if (keyword == MAXRECEIVETIME)   config.setMaximumReceiveTime(value.dval);
  else if (keyword == NOISE_FIGURE)     config.setNoiseFigure(value.dval);
  else if (keyword == DUPLEXSWITCHTIME) config.setDuplexerSwitchTime(value.dval);
  else if (keyword == ANTENNAEGAIN)     config.setAntennaeGain(value.dval);
  else if (keyword == AZIMUTH)          config.setAzimuth(value.dval);
  else if (keyword == BANDWIDTH)        config.setBandWidth(value.dval);
  else if (keyword == AZBEAMWIDTH)      config.setAzBeamWidth(value.dval);
  else if (keyword == ELBEAMWIDTH)      config.setElBeamWidth(value.dval);
  else if (keyword == ADCRESOLUTION)    config.setADCResolution(value.ival);
  else if (keyword == ADCMODE) {
    if (value.sval == "Power")
      config.setADCMode( ADCMode::Power );
    else if (value.sval == "Logarithm")
      config.setADCMode( ADCMode::Logarithm );
    else
      throw invalid_argument(__PRETTY_FUNCTION__ + string(": Unrecognized ADCMode: '") + value.sval + string("'."));
  }
  else if (keyword == ADCMIN2NOISE) config.setADCMin2Noise(value.dval);
  else if (keyword == ADCMAX2NOISE) config.setADCMax2Noise(value.dval);
}


RadarConfigType RadarConfigParser::getKeywordType(const string& keyword) const {
  auto it = key_types.find(keyword);
  if (it == key_types.end())
    throw invalid_argument(__PRETTY_FUNCTION__ + string(": tried to parse unknown keyword '") + keyword + string("'."));
  else
    return it->second;
}



RadarConfigValue RadarConfigParser::getConfigValue(const string& keyword, const vector<string>& tokens) const {
  auto type = getKeywordType(keyword);
  
  RadarConfigValue value;
  string val_str = tokens[1];

  try {
     switch(type) {
       case RadarConfigType::integer: if (is_int(val_str)) 
                                        {value.ival = stoi(val_str); break;}
                                      else
                                        throw invalid_argument("");
       case RadarConfigType::fdouble: value.dval = stod(val_str); break;
       case RadarConfigType::fstring: value.sval = val_str; break;
       default: throw logic_error(__PRETTY_FUNCTION__ + string(": unknonwn failure while parsing keyword."));
     }
   } 
   catch (invalid_argument e) {
     throw invalid_argument(__PRETTY_FUNCTION__ + string(": failed to read value of keyword '") + keyword + string("'."));
   }

   return value;
}


void RadarConfigParser::parseKeyword(RadarConfig& config, set<string>& set_keys, const std::vector<std::string>& tokens) const {

  string keyword = tokens[0];  

  if (set_keys.count(keyword))
    throw invalid_argument(__PRETTY_FUNCTION__ + string(": tried to parse keyword '") + keyword + string("' more than once."));

  if (tokens.size() != 2)
    throw invalid_argument(__PRETTY_FUNCTION__ + string(": keyword '") + keyword + string("' has wrong number of arguments."));

  RadarConfigValue value = getConfigValue(keyword, tokens);
  set_keys.insert(keyword);
  setKeyordParams(config, keyword, value);
}


RadarConfig RadarConfigParser::parseStream(std::istream& in) const {
  string line;

  RadarConfig config;
  set<string> set_keys;

  while( getline(in, line) ) {
    auto tokens = splitLine(line);
    if (tokens.size() == 0) continue;

    parseKeyword(config, set_keys, tokens);
  }
  return config;
}


RadarConfigParser:: RadarConfigParser() :
  key_types( createKeyTypes() )
{}


RadarConfig RadarConfigParser::parseFile(const string& filename) const {
  ifstream in(filename);
  if (!in)
    throw invalid_argument(__PRETTY_FUNCTION__ + string(": ConfigParser failed to read file: " + filename));
  return parseStream(in);
}


RadarConfig RadarConfigParser::parseString(const string& config_str) const {
  istringstream in(config_str);
  return parseStream(in);
}

}
