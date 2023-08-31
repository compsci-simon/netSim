#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <thread>

#include "switch.h"
#include "../protocols/ethernet.h"


void f() {
  std::cout << "test" << std::endl;
}


Switch::Switch() {
  frame_queue = std::queue<void*>();
  ports = std::vector<Port>();
}

void Switch::switch_on() {
  sockaddr_in server_address;
  sockaddr client_address;
  int server_sock;
  socklen_t addr_length;

  addr_length = sizeof(server_address);

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0) {
    std::cerr << "Error creating socket" << std::endl;
    return;
  }
  // Bind socket to IP and port
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(1234);
  if (bind(server_sock, (struct sockaddr *)&server_address, addr_length) < 0) {
    std::cerr << "Error binding socket" << std::endl;
    return;
  }

  if (listen(server_sock, 3) < 0) {
    std::cerr << "Listen failed" << std::endl;
    return;
  }

  std::cout << "Accepting connections" << std::endl;
  while (true) {
    int socket = accept(server_sock, &client_address, &addr_length);
    char ipAddress[INET_ADDRSTRLEN];
    std::cout << "Accepted connection" << std::endl;
    if (socket < 0) {
      std::cerr << "Accept failed" << std::endl;
      break;
    } else {
      Port port;
      port.socket = socket;
      // port.thread = std::thread([this, socket]() {
      //   this->handle_port_traffic(socket);
      // });
      // handle_port_traffic(socket);
      // std::thread(&Switch::test, this);
      port.thread = std::thread(f);
      ports.push_back(port);
      break;
    }
  }
  for (int i = 0; i < ports.size(); i++) {
    ports.at(i).thread.join();
    close(ports.at(i).socket);
  }
  close(server_sock);
}

void Switch::handle_port_traffic(int socket) {
  const auto BUFFER_SIZE {13734};
  unsigned char buffer[BUFFER_SIZE] {0};
  auto bytesRead {0};
  while (true) {
    memset(buffer, 0, BUFFER_SIZE);
    bytesRead = read(socket, buffer, BUFFER_SIZE);
    if (bytesRead <= 0) {
      break;
    }
    Ethernet* frame = new Ethernet();
    frame->instantiate_from_bit_string(buffer);
    std::cout << "Received frame from " << frame->address_to_string(true) << " to " << frame->address_to_string(false) << std::endl;
    frame_queue.push(frame);
  }
}

void Switch::test() {
  std::cout << "Hello from thread " << std::this_thread::get_id() << std::endl;
}