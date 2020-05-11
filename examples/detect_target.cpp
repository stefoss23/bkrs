#include <iostream>
#include <string>
#include <vector>

#include <radsim/mathematics/math_vector.hpp>
#include <radsim/radar/target.hpp>
#include <radsim/radar/radar.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>

using namespace std;
using namespace radsim;

//Target detection using a CFAR detection algorithm   
double detectTargetDistance(const Radar& radar, 
                           const PulseData& pulse_data,
                           int N, int b, double T) {
  const auto& reg = pulse_data.registry;

  size_t i = N + b;                  //minimum range bin to perform CFAR
  size_t i_max = reg.size() - N - b; //max range bin for CFAR

  int sum_left = 0;
  size_t j_min_left = i - N - b;
  size_t j_max_left = i - b;
  for (size_t j = j_min_left ; j < j_max_left; j++)
    sum_left += reg[j];

  int sum_right = 0;
  size_t j_min_right = i + b + 1;
  size_t j_max_right = i + b + N + 1;
  for (size_t j = j_min_right; j < j_max_right; j++)
    sum_right += reg[j];

  int num_bins = 0;
  double sum_range = 0;
  while ( i <= i_max ) {
    double threshold = T * (sum_left + sum_right) / (2.0 * N);
    if (reg[i] > threshold) {
      sum_range += radar.getRange(i); //m
      num_bins++;
    }
    sum_left  += reg[j_max_left]  - reg[j_min_left];
    sum_right += reg[j_max_right] - reg[j_min_right];
    j_min_left++; j_max_left++; j_min_right++; j_max_right++;
    i++;
  }
  return sum_range / (double)num_bins; //m
}


void detectTarget(Radar& radar, const TargetCollection& targets) {
  double t = radar.getCurrentTime();
  auto pulse_data = radar.generatePulseData(targets);  
  double distance = detectTargetDistance(radar, pulse_data, 
                                         15, 3, 22);
  cout << "Time(s): " << t << ", distance(m): " << distance << endl;  
}


int main() {
  ApproxFunction<math_vector> path({0.0, 20.0}, 
                                   {  {0, 10000, 0}, 
                                      {0, 9600, 0}  });

  TargetCollection targets;
  targets.emplace_back(move(path), 50000.0);

  RadarConfigParser parser;
  RadarConfig config = parser.parseFile("stationary_radar.txt");
  Radar radar(config);

  detectTarget(radar, targets);
  detectTarget(radar, targets);
  detectTarget(radar, targets);
  detectTarget(radar, targets);
  detectTarget(radar, targets);
  return 0;
}
