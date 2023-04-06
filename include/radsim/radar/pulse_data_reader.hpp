#ifndef RADAR_PULSE_DATA_READER_HPP
#define RADAR_PULSE_DATA_READER_HPP

class PulseDataReader {
  public:
    PulseDataReader(const std::string filename);
    PulseData read();
    bool eof();
    close();
};

#endif
