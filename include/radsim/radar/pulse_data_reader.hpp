#ifndef RADAR_PULSE_DATA_READER_HPP
#define RADAR_PULSE_DATA_READER_HPP

#include <fstream>

namespace radsim {  

class PulseDataReader {
  private:
    std::ifstream in;

    int readInt() {
      int num;
      in.read(reinterpret_cast<char *>(&num), sizeof(num));
      return num;
    }

    double readDouble() {
      double num;
      in.read(reinterpret_cast<char *>(&num), sizeof(num));
      return num;
    }

  public:
    PulseDataReader(const std::string filename);
    PulseData read();
    bool eof();
    void close();
};

}

#endif
