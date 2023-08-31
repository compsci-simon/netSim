#ifndef _SWITCH_H_
#define _SWITCH_H_
#include <vector>
#include <thread>
#include <queue>
#include <mutex>

struct Port {
  long MAC {0};
  int socket;
  std::thread thread;
};

class Switch {
  std::vector<Port*> ports;
  std::queue<void*> frame_queue;
  bool ON  {false};
  std::mutex ports_mtx;
  std::mutex frame_q_mtx;
public:
  Switch();
  ~Switch();
  void switch_on();
  void handle_port_traffic(int socket);
};

#endif
