

#include <radsim/radar/radar_data_queue.hpp>

using namespace std;

namespace radsim {

DataQueueNode::DataQueueNode(PulseData data_arg) :
  data( move(data_arg) ),
  next( NULL )
{
}


RadarDataQueue::RadarDataQueue() :
  head(NULL),
  tail(NULL),
  pushed_initial(false),
  num_nodes( 0 ),
  num_nodes_main(0)
{}


RadarDataQueue::~RadarDataQueue() {
  empty();
}

void RadarDataQueue::pop__() {
  num_nodes--;
  num_nodes_main--;
  DataQueueNode * ptr = head;
  head = head->next;
  delete ptr;
}

//One ABOLUTELY has to check with size() before using this function!!!
PulseData RadarDataQueue::pop() {
  if (num_nodes_main <= 1)
    throw logic_error(__PRETTY_FUNCTION__ + string(": tried to access empty queue."));    

  auto data = move(head->data);
  pop__();
  return data;
}


bool RadarDataQueue::isEmpty() {
  return (head == NULL);
}

void RadarDataQueue::empty() {
  while (head)
    pop__();
  pushed_initial = false;
}

size_t RadarDataQueue::size() {
  num_nodes_main = num_nodes.load();
  return num_nodes_main;
}


void RadarDataQueue::push_initial( PulseData data ) {

  if (pushed_initial)
    throw logic_error(__PRETTY_FUNCTION__ + string(": this function can only be called once."));

  pushed_initial = true;

  DataQueueNode * node = new DataQueueNode( move(data) );
  head = node;
  tail = node;
  num_nodes++;
}


void RadarDataQueue::push( PulseData data ) {

  if (pushed_initial) {
    DataQueueNode * node = new DataQueueNode( move(data) );
    tail->next = node;
    tail = node;
    num_nodes++;
  }
  else
    throw logic_error(__PRETTY_FUNCTION__ + string(": 'push_initial' must be called first."));
}

}
