#include <iostream>
#include "node.h"

int main(int argc, char* argv[]) {
  // if (argc < 2) {
  //   std::cerr << "You must supply a name for this node" << std::endl;
  //   return 1;
  // }
  Node node(PORT, (char *)"localhost", "simon");
  node.connect_to_router();
  node.start_listen_thread();
  node.dhcp_discover();
  node.disconnect();
  return 0;
}