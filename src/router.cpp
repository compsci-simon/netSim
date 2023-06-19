#include "router.h"
#include "arp.h"
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
  int bytesRead = 0;
  while (true) {
    memset(recv_buffer, 0, BUFFER_SIZE);
    bytesRead = read(clientfd, recv_buffer, BUFFER_SIZE);
    if (bytesRead <= 0) {
      break;
    }

    frame.instantiate_from_bit_string(recv_buffer);
    process_frame(frame);
  }
  
  close(sockfd);
}

void Router::process_frame(Frame frame) {
  if (frame.get_destination_address() == 0xffffffffffff || frame.get_destination_address() == macAddress) {
    if (frame.get_type() == 0x0800) {
      // Process IP Packet
      frame.load_packet(&packet);
      process_packet(packet);
    } else if (frame.get_type() == 0x0806) {
      // Process ARP Query
      Arp query;
      frame.demultiplex(&query);
      process_query(query);
    } else {
      std::cerr << "Received frame with unknow protocol " << frame.get_type_string() << std::endl;
    }
  } else {
    std::cerr << "Received a frame with a destination not equal to router MAC or broadcast MAC. Frame destination address = " << frame.address_to_string(false) << std::endl;
  }
}

void Router::process_packet(Packet packet) {
  if (packet.get_destination() == 0xffffffff || packet.get_destination() == ip_addr) {
    if (packet.get_protocol() == 17) {
      packet.load_datagram(&datagram);
      process_datagram(datagram);
    } else {
      std::cerr << "Received packet with unknown destination protocol " << packet.get_protocol() << std::endl;
    }
  } else {
    char buffer[17] {0};
    Packet::address_to_string(packet.get_destination(), buffer);
    std::cerr << "Received a packet with a destination not equal to router IP or broadcast. Packet destination IP = " << buffer << std::endl;
    // Silently dismiss packet
  }
}

void Router::process_query(Arp query) {
  char buf[17] {0};
  Packet::address_to_string(query.get_target_protocol(), buf);
  std::cout << "Received ARP query targetted for " << buf << std::endl;
  if (query.get_target_protocol() == ip_addr) {
    Frame frame;
    query.set_target_hardware(macAddress);
    frame.set_destination(query.get_source_hardware());
    frame.set_source(macAddress);
    frame.set_type(0x0806);
    frame.get_bit_string(send_buffer);
    send(clientfd, send_buffer, BUFFER_SIZE, 0);
  } else {
    std::cout << "Dismissing ARP query" << std::endl;
  }
}

void Router::process_datagram(Datagram datagram) {
  if (datagram.get_destination_port() == 67) {
    datagram.unencapsulate_dhcp_message(&dhcp_message);
    dhcp_server.handle_message(dhcp_message);
  } else {
    std::cerr << "datagram received with unknown destination port" << datagram.get_destination_port() << std::endl;
    return;
  }
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
  frame.get_bit_string(send_buffer);
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
  frame.get_bit_string(send_buffer);
  send(clientfd, send_buffer, BUFFER_SIZE, 0);
}
