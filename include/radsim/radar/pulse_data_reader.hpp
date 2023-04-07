#ifndef RADAR_PULSE_DATA_READER_HPP
#define RADAR_PULSE_DATA_READER_HPP

namespace radsim {  

class PulseDataReader {
  public:
    PulseDataReader(const std::string filename);
    PulseData read();
    bool eof();
    void close();
};

}

#endif
