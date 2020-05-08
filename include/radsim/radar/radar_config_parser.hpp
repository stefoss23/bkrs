/*

RadarConfigParser may parse a string or a file, and produce a RadarConfig object. 

File or string syntas:
# - marks rest of line as comment, and is ignored by parser
Keyword Value
Keyword value
...
Keyword value

Keyword has one value, which may be integer, double or string.   

*/


#ifndef RADAR_RADAR_CONFIG_PARSER_HPP
#define RADAR_RADAR_CONFIG_PARSER_HPP


#include <string>
#include <map>
#include <set>

#include <radsim/radar/radar_config.hpp>

namespace radsim {

enum class RadarConfigType {nan, fdouble, fstring, integer};

struct RadarConfigValue {
  double dval;
  int ival;
  std::string sval;
};


class RadarConfigParser {
  private:

  std::map<std::string, RadarConfigType> key_types;

  void setKeyordParams(RadarConfig& config, std::string keyword, RadarConfigValue value) const;
  RadarConfigType getKeywordType(const std::string& keyword) const;
  RadarConfigValue getConfigValue(const std::string& keyword, const std::vector<std::string>& tokens) const;
  void parseKeyword(RadarConfig& config, std::set<std::string>& set_keys, const std::vector<std::string>& tokens) const;
  RadarConfig parseStream(std::istream& in) const;

  public:
    RadarConfigParser();

    RadarConfig parseFile(const std::string& filename) const;
    RadarConfig parseString(const std::string& config_string) const;
    
};

}


#endif
