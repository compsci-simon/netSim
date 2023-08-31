#ifndef _SWITCH_H_
#define _SWITCH_H_
#include <vector>
#include <thread>
#include <queue>
#include <mutex>

class Ethernet;

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
  std::mutex arp_table_mtx;
  std::vector<std::vector<long> > mac_table;
  int portId {0};
  unsigned char* send_buffer;
public:
  Switch();
  ~Switch();
  void switch_on();
  void handle_port_traffic(int socket);
  Ethernet* get_frame();
  void send_frame(Ethernet* frame);
  void register_in_arp_table(Ethernet* frame);
};

#endif
