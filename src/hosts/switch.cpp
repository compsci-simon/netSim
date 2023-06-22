#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>
#include <vector>
#include <thread>

#include "switch.h"

const int PORT = 1234;

void Switch::switch_on() {
  sockaddr_in server_address;
  sockaddr client_address;
  int addr_len, server_sock, client_sock;
  socklen_t addr_length;

  addr_len = sizeof(server_address);
  addr_length = sizeof(server_address);
  std::vector<std::thread> threads;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0) {
    std::cerr << "Error creating socket" << std::endl;
    return;
  }
  // Bind socket to IP and port
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(PORT);
  if (bind(server_sock, (struct sockaddr *)&server_address, addr_len) < 0) {
    std::cerr << "Error binding socket" << std::endl;
    return;
  }

  if (listen(server_sock, 3) < 0) {
    std::cerr << "Listen failed" << std::endl;
    return;
  }

  std::cout << "Accepting connections" << std::endl;
  while (true) {
    int temp = accept(server_sock, &client_address, &addr_length);
    char ipAddress[INET_ADDRSTRLEN];
    std::cout << "Accepted connection" << std::endl;
    if (temp < 0) {
      std::cerr << "Accept failed" << std::endl;
      break;
    } else {
      Port port;
      port.socket = temp;
      port.thread = std::thread([this, temp]() {
        this->handle_port_traffic(temp);
      });
    }
  }
  for (int i = 0; i < ports.size(); i++) {
    ports.at(i).thread.join();
  }
}

void Switch::handle_port_traffic(int socket) {

}