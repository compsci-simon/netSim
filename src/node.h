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
const int BUFFER_SIZE = 1600;

class Node {
  int sockfd;
  int port;
  char* host;
  unsigned char recv_buffer[FRAME_SIZE];
  unsigned char send_buffer[FRAME_SIZE] {0};
  struct sockaddr_in serverAddress;
  const char* name;
  bool listen;
  unsigned char macAddress[6] {0};
  Frame frame;
  Packet packet;
  Datagram datagram;
  DHCP_Message dhcp_message;

  int sendMessageToServer(std::string message);
  void receive_messages_from_server();
  void echo_messages_received();
public:
  Node(int port, char *host, const char* name) {
    this->port = port;
    this->host = host;
    this->name = name;
    // generate_mac_address(macAddress);
    macAddress[0] = 0xff;
    macAddress[1] = 0xff;
    macAddress[2] = 0xab;
    macAddress[3] = 0xcd;
    macAddress[4] = 0xef;
    macAddress[5] = 0x01;
  };
  int connect_to_router();
  void main_loop();
  void obtain_ip_address();
  void disconnect();
};

#endif