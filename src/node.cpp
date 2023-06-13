#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <thread>
#include "logging.h"
#include "frame.h"
#include "packet.h"
#include "datagram.h"
#include "dhcp.h"
#include "utils.h"

const int PORT = 12345;
const int BUFFER_SIZE = 1600;

class Node
{
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

int Node::sendMessageToServer(std::string message) {
  return 0;
}

void Node::receive_messages_from_server() {
  memset(recv_buffer, 0, BUFFER_SIZE);
  if (read(sockfd, recv_buffer, BUFFER_SIZE) <= 0) {
    listen = false;
  }
}

void Node::echo_messages_received() {
  while (listen) {
    receive_messages_from_server();
    std::cout << recv_buffer << std::endl;
  }
}

int Node::connect_to_router() {
  // Create socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Error creating socket" << std::endl;
    return 1;
  }

  // Bind socket to port
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(port);
  if (inet_pton(AF_INET, host, &(serverAddress.sin_addr)) < 0) {
    std::cerr << "Invalid address" << std::endl;
    return 1;
  }

  // Connect to server
  socklen_t serverAddressLen = sizeof(serverAddress);
  if (connect(sockfd, (struct sockaddr *) &serverAddress, serverAddressLen) < 0) {
    std::cerr << "Error connecting to the server" << std::endl;
    return 1;
  }
  return 0;
  }

void Node::obtain_ip_address() {

  dhcp_message.set_op(1);
  dhcp_message.set_ciaddr(0);
  dhcp_message.set_giaddr(0);

  datagram.set_source_port(68);
  datagram.set_destination_port(67);
  datagram.set_payload(&dhcp_message);

  packet.set_destination(IP_BROADCAST);
  packet.set_source(0);
  packet.set_payload(datagram);

  frame.set_source(macAddress);
  frame.set_destination((unsigned char*) ETHERNET_BROADCAST_ADDRESS);
  frame.set_payload(packet);
  frame.get_byte_string(send_buffer);
  
  send(sockfd, send_buffer, BUFFER_SIZE, 0);
}

void Node::disconnect() {
  close(sockfd);
}

void Node::main_loop() {
    std::string msg;
    std::thread thread(&Node::echo_messages_received, this);
    listen = true;
    while (listen) {
      std::getline(std::cin, msg);
      sendMessageToServer(msg);
      if (msg == "quit") {
        listen = false;
        break;
      }
    }
    thread.join();
  }

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "You must supply a name for this node" << std::endl;
    return 1;
  }
  Node node(PORT, (char *)"localhost", argv[1]);
  node.connect_to_router();
  node.obtain_ip_address();
  node.disconnect();
  return 0;
}