#ifndef _NODE_H_
#define _NODE_H_
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <vector>

const int PORT = 12345;
const int BUFFER_SIZE = 13734;

class ICMP;
class IP;
class Ethernet;
class Arp;
class Datagram;
class DHCP_Message;
class Logger;

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
  int subnet_mask {0};
  int router_ip {0};
  std::thread receive_thread;
  std::vector<std::vector<long>> arp_table;
  bool perform_router_discover = true;
  int sendMessageToServer(std::string message);
  void receive_messages_from_server();
  Logger* logger;
public:
  Node(int port, char *host, const char* name);
  int connect_to_router();
  void dhcp_discover();
  void dhcp_request(Ethernet source, DHCP_Message message);
  void dhcp_bind(DHCP_Message message);
  void disconnect();
  void set_ip_address(int new_ip) { ipAddress = new_ip; };
  void start_listen_thread();
  void handle_frame();
  void handle_packet(Ethernet frame, IP packet);
  void arp_query(int target_addr);
  void process_arp(Arp query);
  void process_icmp_packets(ICMP packet);
  void handle_datagram(Ethernet frame, Datagram datagram);
  void process_dhcp_message(Ethernet frame, DHCP_Message message);
  void set_router_ip(int ip) { router_ip = ip; }
  void set_subnet_mask(int subnet_mask) { this->subnet_mask = subnet_mask; }
  int get_subnet_mask() { return subnet_mask; }
  int get_router_ip() { return router_ip; }
  void router_solicitation();
  void ping(int target);
};

#endif