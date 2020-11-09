
#include <radsim/utils/timer.hpp>

#include <radsim/radar/target.hpp>
#include <radsim/radar/radar.hpp>
#include <radsim/radar/radar_config.hpp>
#include <radsim/radar/radar_interface.hpp>

using namespace std;
using namespace radsim;

namespace {

  void simulationRunner(Radar& radar,
                        RadarDataQueue& queue,
                        const TargetCollection& targets,
                        double time_step, 
                        atomic<double>& sim_time_atomic, 
                        atomic<bool>& on, 
                        atomic<bool>& allow_send_data, 
                        bool signal_override, 
                        double signal_strength,
                        bool initiated,
                        bool statistics) {

    if (!initiated) {
      radar.reset(0);  //sim_time reset to zero
    }

    double sim_check = time_step; //s, 
    double start_time = radar.getCurrentTime();
    sim_time_atomic.store( start_time ); //s

    //If the queue does not contain some elements, it may falter during oepration
    if (!initiated) {
      queue.push_initial( radar.generatePulseData(targets, signal_override, signal_strength) );
      initiated = true;
    }

    allow_send_data.store(true);  

    double work_time = 0; //s, The time the thread has worked without busy-wait
    Timer timer;
    double current_time = 0;

    while(on.load()) {

      double period_start = timer.elapsed(); //s

      do {
        queue.push( radar.generatePulseData(targets, signal_override, signal_strength) );
      } while (radar.getCurrentTime() < sim_check );

      current_time = radar.getCurrentTime(); //s  
      sim_time_atomic.store(current_time); //s  

      work_time += (timer.elapsed() - period_start);

      double time_status = timer.elapsed() + start_time;
      while (time_status < radar.getCurrentTime()) {
        time_status = timer.elapsed() + start_time;
      } //busy-wait
      //while (timer.elapsed() < radar.getCurrentTime()) {}

      sim_check += time_step; //s
    }
    double end_time = timer.elapsed();

    if (statistics) {
      double frac_work = 100.0 * work_time / radar.getCurrentTime(); //unit, fraction of simulation time spent on actual calculations.
      cout << "Simulation Work Fraction(%): " << frac_work << endl;
    }
   
  }

} //end empty namespace


namespace radsim {


RadarInterface::RadarInterface(const RadarConfig& config, TargetCollection target_collection_arg, double dt) :
  radar( config ),
  target_collection( move(target_collection_arg) ),
  sim_thread(NULL),
  allow_send_data(false),
  on(false),
  statistics(false),
  initiated(false),
  time_step(dt),
  sim_time(0),
  queue_size(0)
{
  min_range = radar.getMinimumRange(); //m
  range_bin = radar.getRangeBin(); //m
}

RadarInterface::~RadarInterface() {
  stop();
}


void RadarInterface::setStatistics(bool set) {
  if (sim_thread)
    throw logic_error(__PRETTY_FUNCTION__ + string(": cannot set Statistics when simulation thread is running."));

  statistics = set;
}


void RadarInterface::setAddNoise(bool set) 
{
  if (sim_thread)
    throw logic_error(__PRETTY_FUNCTION__ + string(": cannot set radar parameters when simulation thread is running."));

  radar.setToAddNoise(set);
}


void RadarInterface::start(bool signal_override, double signal_strength) {

  if (sim_thread)
    throw logic_error(__PRETTY_FUNCTION__ + string(": cannot restart simulator without stopping first."));

  on.store(true);

  sim_thread = new thread(simulationRunner, 
                          ref(radar), ref(queue), 
                          ref(target_collection), 
                          time_step, 
                          ref(sim_time), 
                          ref(on), 
                          ref(allow_send_data), 
                          signal_override, 
                          signal_strength, 
                          initiated,
                          statistics);

  initiated = true;
  
}


//s
double RadarInterface::getSimTime() const {
  if (allow_send_data.load()) {
    double t = sim_time.load();
    return t;
  }
  else
    return 0.0;
}


bool RadarInterface::dataReady() {

  if (queue_size > 1) 
    return true;

  if (allow_send_data.load())
    queue_size = queue.size();

  return (queue_size > 1);
}

PulseData RadarInterface::getData() {

  if (queue_size > 1) {
    queue_size--;
    return queue.pop();
  }
  else
    throw logic_error(__PRETTY_FUNCTION__ + string(": no data in queue. Check dataReady() first."));
}

void RadarInterface::stop() {

  allow_send_data.store(false);  //this stops data exhange before sim is shut down
  on.store(false);          //shutting down sim

  //destroying thread
  if (sim_thread) {
    sim_thread->join();
    delete sim_thread;
    sim_thread = NULL;
  }
}

//m
double RadarInterface::getRange(int bin_index) const {
  return min_range + bin_index * range_bin; //m
}

} //end namespace bkradsim

