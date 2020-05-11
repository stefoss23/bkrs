#include <math.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <radsim/mathematics/math_vector.hpp>
#include <radsim/radar/target.hpp>
#include <radsim/radar/radar_interface.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_config_parser.hpp>

using namespace std;
using namespace radsim;


struct LinearCluster {
  int weight;
  math_vector pos; //[m, m, m]
  double time; //s

  LinearCluster(double t, int w, const math_vector& p) :
    time(t), weight(w), pos(p)
  {}
};

class Cluster {
  private:
    int bin_min;
    bool updated;
    vector<LinearCluster> rad_clusters;

  public:
    Cluster(int bin_min_arg, int bin_max_arg, 
            double t, const math_vector& pos_arg) 
    {
      update(bin_min_arg, bin_max_arg, t, pos_arg);    
    }

    int getMin();
    bool isUpdated();
    void reset();
    void update(int bin_min_arg, int bin_max_arg, 
                double t, const math_vector& pos_arg);
    void printPlot();
};

int Cluster::getMin() {
  return bin_min;
}

bool Cluster::isUpdated() {
  return updated;
}

void Cluster::reset() {
  updated = false;
}

void Cluster::update(int bin_min_arg, int bin_max_arg, 
                     double t, const math_vector& pos_arg) {
  updated = true;
  bin_min = bin_min_arg;
  rad_clusters.emplace_back(t, bin_max_arg + 1 - bin_min_arg, pos_arg);
};

void Cluster::printPlot(){
  double t = 0;
  double x = 0;
  double y = 0;
  double total_weight = 0;
  for (auto rad_cluster : rad_clusters) {
    total_weight += rad_cluster.weight;
    t += rad_cluster.weight * rad_cluster.time;
    x += rad_cluster.weight * rad_cluster.pos[0];
    y += rad_cluster.weight * rad_cluster.pos[1];
  }
  x /= total_weight;
  y /= total_weight;
  t /= total_weight;
  cout << "Plot pos: [" << x << ", " << y << "], t = " << t << endl;
}


class DigitalProcessor {
  private:
    RadarInterface radar;

    //CFAR parameters:
    int N = 20;  //number of samples at each side of CUT
    int b = 3;   //closest cells to CUT not used for threshold
    double T = 30;  //Thresholding parameter

    //Clustering parameters:
    int tolerance = 2;
    list<Cluster> clusters;

    vector<int> generateRawDetections__(const PulseData& pulse_data);
    list<Cluster>::iterator findCluster__(int bin_min);
    void handleCluster__(const PulseData& pulse_data, int cl_min, int cl_max);
    void updateClusters__(const PulseData& pulse_data, const vector<int>& detections);
    void processPulse__();
    void generatePlots__();

  public:
    DigitalProcessor(RadarConfig& config, TargetCollection targets) :
      radar(config, move(targets))
    {}

    void generate(double max_time);
};


//Obtaining raw detections using CFAR
vector<int> DigitalProcessor::
            generateRawDetections__(const PulseData& pulse_data) {
  vector<int> detections;
  const auto& reg = pulse_data.registry;

  size_t i = N + b;                  //minimum range bin 
  size_t i_max = reg.size() - N - b; //max range bin

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

  double M = 2.0 * N;
  double background = (sum_right + sum_left) / M;

  int num_bins = 0;
  while ( i <= i_max ) {
    double threshold = T * background;
    if (reg[i] > threshold)
      detections.push_back(i);

    background += (reg[j_max_left]  - reg[j_min_left] + 
                   reg[j_max_right] - reg[j_min_right]) / M;

    j_min_left++; j_max_left++; j_min_right++; j_max_right++;
    i++;
  }
  return detections;
}

list<Cluster>::iterator DigitalProcessor::findCluster__(int bin_min) {
  auto it = clusters.begin();
  while (it != clusters.end()) {
    if ( abs(it->getMin() - bin_min) <= tolerance) break;
    it++;
  }
  return it;
}

void DigitalProcessor::handleCluster__(const PulseData& pulse_data, 
                                       int cl_min, int cl_max) {
  double t = pulse_data.getStartTime();

  double range = 0.5 * ( radar.getRange(cl_min) + radar.getRange(cl_max) );
  auto pos = range * pulse_data.getBoresight();

  auto it = findCluster__(cl_min);
  if (it == clusters.end())
    clusters.emplace_back(cl_min, cl_max, t, pos);
  else
    it->update( cl_min, cl_max, t, pos );
}

void DigitalProcessor::updateClusters__(const PulseData& pulse_data, 
                                        const vector<int>& detections) {
  
  if (detections.size() > 0) {

    size_t cl_min = detections[0];
    size_t cl_max = detections[0];
    for (int detection : detections) {
      if ( (detection - cl_max) > tolerance ) {
        handleCluster__(pulse_data, cl_min, cl_max);
        cl_min = detection;
      }
      cl_max = detection;
    }
    handleCluster__(pulse_data, cl_min, cl_max);  
  }
}

void DigitalProcessor::generatePlots__() {
  auto it = clusters.begin();
  while (it != clusters.end()) {
    if (it->isUpdated()) {
      it->reset();
      it++;
    }
    else {
      it->printPlot();
      it = clusters.erase(it);
    }
  }
}

void DigitalProcessor::processPulse__() {
  auto pulse_data = radar.getData();
  auto detections = generateRawDetections__(pulse_data);
  updateClusters__(pulse_data, detections);
  generatePlots__();
}

void DigitalProcessor::generate(double max_time) {
  radar.start();

  while (radar.getSimTime() < max_time)
    if (radar.dataReady()) processPulse__();

  radar.stop();
}



int main() {

  TargetCollection targets;
  targets.emplace_back( (math_vector){0, 5000, 0}, 50 );

  RadarConfigParser parser;
  auto config = parser.parseFile("naval_radar.txt");

  DigitalProcessor processor(config, move(targets));
  processor.generate(1.0);

  return 0;
}
