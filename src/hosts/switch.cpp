#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <thread>

#include "switch.h"
#include "router.h"
#include "../protocols/ethernet.h"


Switch::Switch(Router* router) {
  frame_queue = std::queue<void*>();
  send_buffer = new unsigned char[13734] {0};
  this->router = router;
}

Switch::~Switch() {
  ON = false;
  frame_q_mtx.lock();
  while (frame_queue.size() > 0) {
    Ethernet* frame = (Ethernet*) frame_queue.back();
    frame_queue.pop();
    free(frame);
  }
  frame_q_mtx.unlock();
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

  ON = true;
  std::cout << "Accepting connections" << std::endl;
  while (ON) {
    int socket = accept(server_sock, &client_address, &addr_length);
    std::cout << "Accepted connection" << std::endl;
    if (socket < 0) {
      std::cerr << "Accept failed" << std::endl;
      break;
    } else {
      Port* port = new Port();
      port->MAC = 0;
      port->socket = socket;
      std::cout << "Creating thread" << std::endl;
      port->thread = std::thread([this, socket](){
        this->handle_port_traffic(socket);
      });
      std::cout << "Created thread" << std::endl;
      ports_mtx.lock();
      ports.push_back(port);
      ports_mtx.unlock();
      break;
    }
  }
  ports_mtx.lock();
  while (ports.size() > 0) {
    int lastIndex = ports.size() - 1;
    ports.at(lastIndex)->thread.join();
    close(ports.at(lastIndex)->socket);
    Port* x = ports.at(lastIndex);
    free(x);
    ports.pop_back();
  }
  ports_mtx.unlock();
  close(server_sock);
}

void Switch::handle_port_traffic(int socket) {
  const auto BUFFER_SIZE {13734};
  unsigned char buffer[BUFFER_SIZE] {0};
  auto bytesRead {0};
  while (ON) {
    memset(buffer, 0, BUFFER_SIZE);
    bytesRead = read(socket, buffer, BUFFER_SIZE);
    if (bytesRead <= 0) {
      break;
    }
    Ethernet* frame = new Ethernet();
    frame->instantiate_from_bit_string(buffer);
    register_in_arp_table(frame);
    std::cout << "Received frame from " << frame->address_to_string(false) << " to " << frame->address_to_string(true) << std::endl;
    frame_q_mtx.lock();
    frame_queue.push(frame);
    frame_q_mtx.unlock();
    std::thread([this]() {
      this->router->interrupt(Interrupt::FRAME_RECEIVED);
    });
    break;
  }
}

void Switch::register_in_arp_table(Ethernet* frame) {
  long source_mac = frame->get_source_address();
  bool found = false;
  arp_table_mtx.lock();
  for (int i = 0; i < mac_table.size(); i++) {
    if (mac_table.at(i).at(0) == source_mac) {
      found = true;
      break;
    }
  }
  if (!found) {
    std::vector<long> entry;
    entry.push_back(source_mac);
    entry.push_back(portId++);
    mac_table.push_back(entry);
  }
  arp_table_mtx.unlock();
}

Ethernet* Switch::get_frame() {
  Ethernet* frame = nullptr;
  frame_q_mtx.lock();
  if (frame_queue.size() > 0) {
    frame = (Ethernet*) frame_queue.front();
    frame_queue.pop();
  }
  frame_q_mtx.unlock();
  return frame;
}

void Switch::send_frame(Ethernet* frame) {
  long destination_address = frame->get_destination_address();
  bool found = false;
  arp_table_mtx.lock();
  for (int i = 0; i < mac_table.size(); i++) {
    if (mac_table.at(i).at(0) == destination_address) {
      int port = mac_table.at(i).at(1);
      ports_mtx.lock();
      memset(send_buffer, 0, 13734);
      frame->get_bit_string(send_buffer);
      send(ports.at(port)->socket, send_buffer, 13734, 0);
      ports_mtx.unlock();
      found = true;
      break;
    }
  }
  if (!found) {
    for (int i = 0; i < ports.size(); i++) {
      memset(send_buffer, 0, 13734);
      frame->get_bit_string(send_buffer);
      send(ports.at(i)->socket, send_buffer, 13734, 0);
    }
  }
  arp_table_mtx.unlock();
}
