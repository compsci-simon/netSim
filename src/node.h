#ifndef _NODE_H_
#define _NODE_H_
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <thread>
#include "frame.h"
#include "packet.h"
#include "datagram.h"
#include "dhcp.h"

const int PORT = 12345;
const int BUFFER_SIZE = 13734;

class Node {
  int sockfd;
  int port;
  char* host;
  unsigned char recv_buffer[BUFFER_SIZE] {0};
  unsigned char send_buffer[BUFFER_SIZE] {0};
  struct sockaddr_in serverAddress;
  const char* name;
  bool listen;
  long int macAddress {0};
  int ipAddress {0};
  std::thread receive_thread;
  Frame frame;
  Packet packet;
  Datagram datagram;
  DHCP_Message dhcp_message;

  int sendMessageToServer(std::string message);
  void receive_messages_from_server();
public:
  Node(int port, char *host, const char* name) {
    this->port = port;
    this->host = host;
    this->name = name;
    // generate_mac_address(macAddress);
    macAddress = 998877665544;
  };
  int connect_to_router();
  void dhcp_discover();
  void dhcp_request(DHCP_Message message);
  void dhcp_bind(DHCP_Message message);
  void arp(int address);
  void disconnect();
  void set_ip_address(int new_ip) { ipAddress = new_ip; };
  void start_listen_thread();
  void handle_frame();
};

#endif