/*
The RadarCom object is the interface between the radar and the processing during real time data processing. 

The RadarCom object own the radar object. 

It is initialized by:

RadarComType com(RadarConfig, target_collection):

Simulation is started bv:
com.start() in which data will regularly be inserted into a queue and are readily available. 

com.stop() stops the simulation. 
*/

#ifndef RADAR_INTERFACE_HPP
#define RADAR_INTERFACE_HPP

#include <vector>
#include <thread>
#include <atomic>

#include <radsim/radar/target.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_data_queue.hpp>
#include <radsim/radar/radar.hpp>

namespace radsim {

class RadarInterface {

  Radar radar;
  TargetCollection target_collection;

  std::thread * sim_thread;
  RadarDataQueue queue;

  std::atomic<double> sim_time; //s, the "Clock" of the simulator
  std::atomic<bool> allow_send_data; //allows for sending data from sim to process    
  std::atomic<bool> on;            //on tells the sim_runner to continue running

  bool statistics;   //If true, at end of a sim_run, Percentage work time is printed, 
  bool initiated;  //if true, has initiated the sim
  double time_step;    //s, time_step in simulation before updating sim_time;
  size_t queue_size; //number of elements in queue is at least this number

  //radar parameters
  double min_range; //m
  double range_bin; //m

  public:
    RadarInterface(const RadarConfig& config, TargetCollection target_collection_, double dt = 0.15);
    //dt: s, timestep in the simulation, before updating sim_time. 

    RadarInterface(const RadarInterface& other) = delete;
    RadarInterface& operator=(const RadarInterface& other) = delete;

    ~RadarInterface();

    void setStatistics(bool set);
    //set: if true, A printout of how much simulator thread worked, is printed. Default set = false

    void setAddNoise(bool set);
    //set: if yes: raadar receiver noise is added to simulation, default is true

    void start(bool signal_override = false, double signal_strength = 0);
    //signal_override: if yes, then received signal is signal_strength.
    //signal_strength = 0

    void stop();
    
    double getSimTime() const; //s

    bool dataReady();
    PulseData getData();
    double getRange(int bin_index) const; //m

};

}

#endif
