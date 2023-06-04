#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <thread>
#include <arpa/inet.h>

const int PORT = 12345;
const int BUFFER_SIZE = 1024;


void handleConnection(int sockfd) {
  std::cout << "Handling connection for socket " << sockfd << std::endl;
  while (true) {
    // Receive and print messages
    int valRead;
    char buffer[BUFFER_SIZE];
    valRead = read(sockfd, buffer, BUFFER_SIZE);
    if (valRead < 0) {
      std::cerr << "Error reading value" << std::endl;
      continue;
    }
    std::cout << "Received from client " << sockfd << ": " << buffer << std::endl;
    send(sockfd, buffer, BUFFER_SIZE, 0);
  }
  close(sockfd);
}

class Router {
private:
  int sockfd;
  struct sockaddr_in serverAddress, clientAddress;
  std::vector<int> clients;
  std::vector<int> threads;
public:
  bool accept_connections() {
    int addrLen = sizeof(serverAddress);
    std::vector<std::thread> threads;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      std::cerr << "Error creating socket" << std::endl;
      return false;
    }
    // Bind socket to IP and port
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr *)&serverAddress, addrLen) < 0) {
      std::cerr << "Error binding socket" << std::endl;
      return false;
    }

    if (listen(sockfd, 3) < 0) {
      std::cerr << "Listen failed" << std::endl;
      return false;
    }

    socklen_t client_len = sizeof(clientAddress);
    std::cout << "Accepting connections" << std::endl;
    while (true) {
      int temp = accept(sockfd, (struct sockaddr *)&clientAddress, &client_len);
      char ipAddress[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(serverAddress.sin_addr), ipAddress, INET_ADDRSTRLEN);
      uint16_t port = ntohs(serverAddress.sin_port);
      std::cout << "Accepted connection" << std::endl;
      std::cout << "Address: " << ipAddress << std::endl;
      std::cout << "Port: " << port << std::endl;
      if (temp < 0) {
        std::cerr << "Accept failed" << std::endl;
        return false;
      } else {
        clients.push_back(temp);
        threads.emplace_back(handleConnection, temp);
      }
    }
    for (int i = 0; i < threads.size(); i++) {
      threads.at(i).join();
    }
  }
};
int main()
{
  Router router;
  router.accept_connections();
  return 0;
}