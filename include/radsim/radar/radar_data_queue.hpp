/*
The RadarDataQueue is meant to be used simultaneously be two threads:
- Radar Simulation thread: pushing data
- Processing thread: popåing data

The idea is thay both threads work at opposite ends of the queue, and therefore
usually avoid race conditions. 
*/

#ifndef RADAR_REG_QUEUE_HPP
#define RADAR_REG_QUEUE_HPP

#include <memory>
#include <atomic>

#include <radsim/radar/pulse_data.hpp>

namespace radsim {

struct DataQueueNode {
  PulseData data;
  DataQueueNode * next;

  DataQueueNode(PulseData data_arg);

};

class RadarDataQueue {

  DataQueueNode * head;
  DataQueueNode * tail;
  std::atomic<size_t> num_nodes;
  bool pushed_initial;
  size_t num_nodes_main; //used for counting nodes in the main thread

  void pop__();


  public:
    RadarDataQueue();
    ~RadarDataQueue();

    PulseData pop(); //One ABOLUTELY has to check with size() before using this function!!!

    bool isEmpty();
    void empty(); //empties the dataqueue
    size_t size(); //the number of elements is equal or greater than the return value
    void push_initial( PulseData data );  //Used only if queue is empty
    void push( PulseData data ); //Cannot be used for initial push, queue cannot be empty
    
};

}

#endif
