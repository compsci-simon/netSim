#include <iostream>
#include <chrono>
#include "hosts/node.h"

int main(int argc, char* argv[]) {
  // if (argc < 2) {
  //   std::cerr << "You must supply a name for this node" << std::endl;
  //   return 1;
  // }
  Node node(PORT, (char *)"localhost", "simon");
  node.connect_to_router();
  node.start_listen_thread();
  node.dhcp_discover();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  node.ping(node.get_router_ip());
  std::this_thread::sleep_for(std::chrono::seconds(2));
  node.disconnect();
  return 0;
}