
#ifndef RADAR_PULSE_DATA_WRITER_HPP
#define RADAR_PULSE_DATA_WRITER_HPP

/*
Filestructure:

File Version                    (int): 4 bytes
----------------------------------------------
For each pulse data object:

Time                    (s)  (double): 8 bytes
Boresight X            (unit)(double): 8 bytes
Boresight Y            (unit)(double): 8 bytes
Boresight Z            (unit)(double): 8 bytes
Num range bins               (int)   : 4 bytes
Signals                      (short) : 2 bytes x N bins
*/


#include <iostream>
#include <fstream>

namespace radsim {

class PulseDataWriter {
    std::ofstream ofs;

    template <class T>
    void write(T number) {
      ofs.write((char *) &number, sizeof number);
    }

    bool is_closed;

  public:
    PulseDataWriter(const std::string& filename);
    void close();
    void write(const PulseData& pulse_data);
};

}

#endif
