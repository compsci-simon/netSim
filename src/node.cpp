#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include "logging.h"
#include <thread>
#include <random>
#include <iomanip>

const int PORT = 12345;
const int BUFFER_SIZE = 1024;

class Node
{
  int sockfd;
  int port;
  char* host;
  char buffer[BUFFER_SIZE];
  char* send_buffer;
  struct sockaddr_in serverAddress;
  const char* name;
  bool listen;

  int sendMessageToServer(std::string message);
  char* receive_messages_from_server();
  void echo_messages_received();
public:
  Node(int port, char *host, const char* name) {
    this->port = port;
    this->host = host;
    this->name = name;
  };
  int connect_to_server();
  void main_loop();
};

int Node::sendMessageToServer(std::string message) {
  int msg_len = message.size();
  send_buffer = new char[msg_len];
  strcpy(send_buffer, message.c_str());
  send(sockfd, send_buffer, msg_len, 0);
  return 0;
}

char* Node::receive_messages_from_server() {
  memset(buffer, 0, BUFFER_SIZE);
  if (read(sockfd, buffer, BUFFER_SIZE) <= 0) {
    listen = false;
  }
  return buffer;
}

void Node::echo_messages_received() {
  while (listen) {
    char* msg = receive_messages_from_server();
    std::cout << msg << std::endl;
  }
}

int Node::connect_to_server() {

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
  // Node *node;
  // node = new Node(PORT, (char *)"localhost", argv[1]);
  // node->connect_to_server();
  // node->main_loop();
  // delete node;
  std::random_device seed;
  std::mt19937 gen(seed());
  std::uniform_int_distribution<> dis(0, 15);
  int randomNumber = dis(gen);
  std::cout << std::hex << randomNumber << std::endl;
  return 0;
}