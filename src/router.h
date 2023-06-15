#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <arpa/inet.h>
#include <random>
#include "frame.h"
#include "packet.h"
#include "datagram.h"

const int PORT = 12345;
const int BUFFER_SIZE = 1600;

class Router {
private:
  int sockfd;
  struct sockaddr_in serverAddress, clientAddress;
  std::vector<int> clients;
  std::vector<int> threads;
  std::mutex mtx;
  static void handleConnection(int socketfd, Router *router);
  unsigned char send_buffer[1526] {0};
  unsigned char recv_buffer[1526] {0};
  int ip_addr = 0b11000000'10100100'00000000'00000001;
  long int macAddress {0};
  int clientfd;
public:
  Frame frame;
  Packet packet;
  Datagram datagram;
  DHCP_Message dhcp_message;
  DHCP_Server dhcp_server;

  Router();
  bool accept_connections();
  void broadcast(char *msg);
  void send_frame(Frame frame);
  void handleConnection();
  int get_ip_addr();
  void set_self_as_frame_source();
  void send_frame();
};

#endif