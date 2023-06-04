#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <string>

const int PORT = 12345;
const int BUFFER_SIZE = 1024;

class Node
{
  int sockfd;
  int port;
  char* host;
  char buffer[BUFFER_SIZE];
  struct sockaddr_in serverAddress;

  int sendMessageToServer(std::string message);
  std::string receivMessageFromServer();
public:
  Node(int port, char *host) {
    Node::port = port;
    Node::host = host;
  };
  int connect_to_server();
  void main_loop();
};

int Node::sendMessageToServer(std::string message) {
  send(sockfd, (const char*)&message, message.length(), 0);
  return 0;
}

std::string Node::receivMessageFromServer() {
  read(sockfd, buffer, BUFFER_SIZE);
  std::string str(buffer);
  return str;
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
    while (true) {
      std::cout << "Enter message: ";
      std::string msg;
      std::getline(std::cin, msg);
      std::cout << std::endl;
      sendMessageToServer(msg);
      std::string from_server = receivMessageFromServer();
      std::cout << "Received from server: " << from_server << std::endl;
    }
  }

int main() {
  Node *node;
  node = new Node(PORT, (char *)"localhost");
  node->connect_to_server();
  node->main_loop();
  delete node;
  return 0;
}