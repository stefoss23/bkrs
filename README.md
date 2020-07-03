# BKRadSim 
Signal level radar simulator tool for tracking algorithm development

BKRadSim is a radar signal level simulation software library specifically designed
for naval radars. Its end product is the raw signal sampling after reception
and analog to digital conversion, which can then be used as input to a digital
processing algorithm designed by the user. The main motivation behind the
library is to create a tool for learning or developing tracking algorithms. It is
mainly written for C++14 by with bindings to Python as well. The simulator
may be called sequentially, but also parallell to any digital processing in order
to test real time track performance.

     
<img src="https://github.com/stefoss23/librads/blob/master/latex/noise_target_3_c.png" alt="output"
 	title="measurements" width="500" height="250" />
     
*10-bit signal measurements as function of time during reception of a single pulse.*

System requirements:
* Linux OS
* Dual Core, 2 Ghz


Access to manual in directory 'latex'.
Type:
```bash
pdflatex Manual.tex
```

Example code using library:
The following lines of code produce the resultant sampled signal measurements from a rotating radar sequentially emitting and receiving pulses.  
```c++
#include <string>
#include <vector>

#include <radsim/mathematics/math_vector.hpp>
#include <radsim/radar/target.hpp>
#include <radsim/radar/radar_interface.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>

using namespace radsim;

int main() {
  double simulation_time = 2.0 //seconds

  TargetCollection targets;
  targets.emplace_back({0, 10000, 0}, 50000.0);

  RadarConfigParser parser;
  RadarConfig config = parser.parseFile("naval_radar.txt");
  RadarInterface radar(config, move(targets);
  
  radar.start();
  while (radar.getSimTime() < simulation_time) {
    if (radar.dataReady()) {
      auto pulse_data = radar.getData();
      vector<unsigned short>& reg = pulse_data.registry;
    }
  }
  radar.stop();

  return 0;
}
```
Each ```c++ std::vector reg``` is the result of a single pulse emission and reception cycle, similarly to the the above figure.. The signal measurements (*reg*) can be fed into a digital processing algorithm. This could initially be a Constant False Alarm Rate (CFAR) process as defined by the user to create per-scan detections, or *plots*, which again would be fed into a tracking algorithm. 

BKRadsim is does not provide any digital processing assistance, but is a tool for its study and development. 

