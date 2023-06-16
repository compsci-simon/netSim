#include "router.h"
// const unsigned char IP[4] = { 192, 168, 0, 1 };

Router::Router() {
  macAddress = 0x0001234455667;
  dhcp_server.set_router(this);
}

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
  clientfd = accept(sockfd, (struct sockaddr *)&clientAddress, &client_len);
  char ipAddress[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(serverAddress.sin_addr), ipAddress, INET_ADDRSTRLEN);
  uint16_t port = ntohs(serverAddress.sin_port);
  std::cout << "Accepted connection" << std::endl;
  std::cout << "Address: " << ipAddress << std::endl;
  std::cout << "Port: " << port << std::endl;
  
  return true;
}

void Router::handleConnection() {
  memset(recv_buffer, 0, 1526);
  read(clientfd, recv_buffer, 1526);

  frame.load_frame_from_string(recv_buffer);
  frame.load_packet(&packet);

  packet.load_datagram(&datagram);

  if (datagram.get_destination_port() == 67) {
    datagram.unencapsulate_dhcp_message(&dhcp_message);
    dhcp_server.handle_message(dhcp_message);
  }

  memset(recv_buffer, 0, 1526);
  read(clientfd, recv_buffer, 1526);

  frame.load_frame_from_string(recv_buffer);
  frame.load_packet(&packet);

  packet.load_datagram(&datagram);

  if (datagram.get_destination_port() == 67) {
    datagram.unencapsulate_dhcp_message(&dhcp_message);
    dhcp_server.handle_message(dhcp_message);
  }
  
  close(sockfd);
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

void Router::send_frame(Frame frame) {
  frame.get_byte_string(send_buffer);
}

/*
This method is used to set the source 
of the router's frame to the router's
MAC address.
*/
void Router::set_self_as_frame_source() {
  this->frame.set_source(macAddress);
}

/*
Getter for the router's IP address.w
*/
int Router::get_ip_addr() {
  return ip_addr;
}

/*
This method is used to send the router's
frame.
*/
void Router::send_frame() {
  frame.get_byte_string(send_buffer);
  send(clientfd, send_buffer, 1526, 0);
}
