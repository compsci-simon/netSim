#ifndef _SWITCH_H_
#define _SWITCH_H_
#include <vector>
#include <thread>

struct Port {
  long MAC {0};
  int socket;
  std::thread thread;
};

class Switch {
  std::vector<Port> ports;
public:
  void switch_on();
  void handle_port_traffic(int socket);
};

#endif
