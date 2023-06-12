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
  unsigned char macAddress[6] {0};
  static void handleConnection(int socketfd, Router *router);
  unsigned char send_buffer[FRAME_SIZE] {0};
  unsigned char recv_buffer[FRAME_SIZE] {0};
public:
  Frame frame;
  Packet packet;
  Datagram datagram;
  DHCP_Message dhcp_message;

  Router();
  bool accept_connections();
  void broadcast(char *msg);
  void send_frame(Frame frame);
  void handleConnection(int clientfd);
};

#endif