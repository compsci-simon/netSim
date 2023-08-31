#ifndef _SWITCH_H_
#define _SWITCH_H_
#include <vector>
#include <thread>
#include <queue>

struct Port {
  long MAC {0};
  int socket;
  std::thread thread;
};

class Switch {
  std::vector<Port*> ports;
  std::queue<void*> frame_queue;
  bool ON  {false};
public:
  Switch();
  ~Switch();
  void switch_on();
  void handle_port_traffic(int socket);
};

#endif
