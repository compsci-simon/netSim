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

const int PORT = 12345;
const int BUFFER_SIZE = 13734;

class Ethernet;
class IP;
class Arp;
class ICMP;
class Datagram;
class DHCP_Server;

class Router {
private:
  int sockfd;
  struct sockaddr_in serverAddress, clientAddress;
  std::vector<int> clients;
  std::vector<int> threads;
  std::mutex mtx;
  static void handleConnection(int socketfd, Router *router);
  int ip_addr = 0b11000000'10101000'00000000'00000001;
  long int macAddress {0};
  DHCP_Server* dhcp_server;
public:
  int clientfd;
  unsigned char send_buffer[BUFFER_SIZE] {0};
  unsigned char recv_buffer[BUFFER_SIZE] {0};
  Router();
  bool accept_connections();
  void broadcast(char *msg);
  void handleConnection();
  int get_ip_addr() { return ip_addr; }
  void send_frame(Ethernet frame);
  void process_frame(Ethernet frame);
  void process_packet(Ethernet frame, IP packet);
  void process_query(Ethernet frame, Arp query);
  void process_message(Ethernet frame, ICMP message);
  void process_datagram(Ethernet frame, Datagram datagram);
  long get_mac_address() { return macAddress; }
};

#endif