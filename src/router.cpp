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

const int PORT = 12345;
const int BUFFER_SIZE = 1600;
// const unsigned char IP[4] = { 192, 168, 0, 1 };

class Router {
private:
  int sockfd;
  struct sockaddr_in serverAddress, clientAddress;
  std::vector<int> clients;
  std::vector<int> threads;
  std::mutex mtx;
  unsigned char available_ips[255] {0};
  static void handleConnection(int socketfd, Router *router);
public:
  Router() {
    for (int ip = 0; ip < 256; ip++) {
      available_ips[ip] = 1;
    }
  };
  bool accept_connections();
  void broadcast(char *msg);
};

bool Router::accept_connections() {
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
  // while (true) {
  //   int temp = accept(sockfd, (struct sockaddr *)&clientAddress, &client_len);
  //   char ipAddress[INET_ADDRSTRLEN];
  //   inet_ntop(AF_INET, &(serverAddress.sin_addr), ipAddress, INET_ADDRSTRLEN);
  //   uint16_t port = ntohs(serverAddress.sin_port);
  //   std::cout << "Accepted connection" << std::endl;
  //   std::cout << "Address: " << ipAddress << std::endl;
  //   std::cout << "Port: " << port << std::endl;
  //   if (temp < 0) {
  //     std::cerr << "Accept failed" << std::endl;
  //     return false;
  //   } else {
  //     clients.push_back(temp);
  //     threads.emplace_back(handleConnection, temp, this);
  //   }
  // }
  // for (int i = 0; i < threads.size(); i++) {
  //   threads.at(i).join();
  // }
  int clientfd = accept(sockfd, (struct sockaddr *)&clientAddress, &client_len);
  char ipAddress[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(serverAddress.sin_addr), ipAddress, INET_ADDRSTRLEN);
  uint16_t port = ntohs(serverAddress.sin_port);
  std::cout << "Accepted connection" << std::endl;
  std::cout << "Address: " << ipAddress << std::endl;
  std::cout << "Port: " << port << std::endl;

  unsigned char recv_buf[FRAME_SIZE] {0};
  unsigned char payload_buf[PACKET_PAYLOAD_SIZE] {0};
  
  Frame ethernet_frame;
  Packet ip_packet;
  memset(recv_buf, 0, FRAME_SIZE);
  read(clientfd, recv_buf, FRAME_SIZE);
  ethernet_frame.load_frame_from_string(recv_buf);
  ethernet_frame.load_packet(&ip_packet);
  ip_packet.get_payload(payload_buf);

  std::cout << "Received an IP packet with the following payload: " << payload_buf << std::endl;
  close(sockfd);
  return true;
}

void Router::handleConnection(int socketfd, Router *router) {
  std::cout << "Handling connection for socket " << socketfd << std::endl;
  char buffer[BUFFER_SIZE];
  Frame ethernet_frame;
  int bytesRead = 0;

  while (true) {
    memset(buffer, 0, BUFFER_SIZE);
    bytesRead = read(socketfd, buffer, BUFFER_SIZE);
    if (bytesRead < 0)  { 
      std::cerr << "Error reading value" << std::endl;
      break;
    }

    std::cout << "Received from client " << socketfd << ": " << buffer << std::endl;
    if (strcmp(buffer, "quit") == 0) {
      break;
    }
    router->broadcast(buffer);
  }
  close(socketfd);
}

void Router::broadcast(char *msg) {
  mtx.lock();
  for (int i = 0; i < clients.size(); i++) {
    int sfd = clients.at(i);
    send(sfd, msg, strlen(msg), 0);
  }
  mtx.unlock();
}

int main()
{
  Router router;
  router.accept_connections();
  return 0;
}