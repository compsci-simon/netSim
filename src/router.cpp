#include "router.h"
#include "dhcp.h"
#include "arp.h"
#include "ethernet.h"
#include "ip.h"
#include "icmp.h"
#include "datagram.h"
// const unsigned char IP[4] = { 192, 168, 0, 1 };

Router::Router() {
  macAddress = 0x0001234455667;
  this->dhcp_server = new DHCP_Server();
  this->dhcp_server->set_router(this);
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
  Ethernet frame;
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

void Router::process_frame(Ethernet frame) {
  if (frame.get_destination_address() == 0xffffffffffff || frame.get_destination_address() == macAddress) {
    if (frame.get_type() == 0x0800) {
      IP packet;
      // Process IP Packet
      frame.decapsulate(&packet);
      process_packet(frame, packet);
    } else if (frame.get_type() == 0x0806) {
      // Process ARP Query
      Arp query;
      frame.decapsulate(&query);
      process_query(frame, query);
    } else {
      std::cerr << "Received frame with unknow protocol " << frame.get_type_string() << std::endl;
    }
  } else {
    std::cerr << "Received a frame with a destination not equal to router MAC or broadcast MAC. Ethernet destination address = " << frame.address_to_string(false) << std::endl;
  }
}

void Router::process_packet(Ethernet frame, IP packet) {
  if (packet.get_destination() == 0xffffffff || packet.get_destination() == ip_addr 
      || packet.get_destination() == 0xe0000001) {
    if (packet.get_protocol() == 1) {
      ICMP message;
      packet.unencapsulate(&message);
      process_message(frame, message);
    } else if (packet.get_protocol() == 17) {
      Datagram datagram;
      packet.load_datagram(&datagram);
      process_datagram(frame, datagram);
    } else {
      std::cerr << "Received packet with unknown destination protocol " << packet.get_protocol() << std::endl;
    }
  } else {
    char buffer[17] {0};
    IP::address_to_string(packet.get_destination(), buffer);
    std::cerr << "Received a packet with a destination not equal to router IP or broadcast. Packet destination IP = " << buffer << std::endl;
    // Silently dismiss packet
  }
}

void Router::process_query(Ethernet frame, Arp query) {
  char buf[17] {0};
  IP::address_to_string(query.get_target_protocol(), buf);
  std::cout << "Received ARP query targetted for " << buf << std::endl;
  if (query.get_target_protocol() == ip_addr) {
    Ethernet frame;
    query.set_target_hardware(macAddress);

    frame.encapsulate(query);
    frame.set_destination_address(query.get_source_hardware());
    frame.set_source_address(macAddress);
    frame.set_type(0x0806);
    frame.get_bit_string(send_buffer);
    send(clientfd, send_buffer, BUFFER_SIZE, 0);
    std::cout << "Sending ARP reply" << std::endl;
  } else {
    std::cout << "Dismissing ARP query." << std::endl;
  }
}

void Router::process_message(Ethernet source_frame, ICMP message) {
  ICMP new_message;
  IP new_packet, old_packet;
  Ethernet new_frame;
  if (message.get_type() == 0) {
    // PING Reply
    source_frame.decapsulate(&old_packet);
    std::cout << "Received ping reply from " << old_packet.address_to_string(true) << std::endl;
  } if (message.get_type() == 8) {
    // PING Request
    source_frame.decapsulate(&old_packet);

    std::cout << "Received PING from " << old_packet.address_to_string(true) << std::endl;

    new_message.set_type(0);
    new_message.set_code(0);

    new_packet.encapsulate(new_message);
    new_packet.set_destination(old_packet.get_source());
    new_packet.set_source(ip_addr);
    new_packet.set_protocol(1);

    new_frame.encapsulate(new_packet);
    new_frame.set_source_address(macAddress);
    new_frame.set_destination_address(source_frame.get_source_address());
    new_frame.set_type(0x0800);
    new_frame.get_bit_string(send_buffer);
    
    send(clientfd, send_buffer, BUFFER_SIZE, 0);

  } else if (message.get_type() == 10) {
    // Router solicitation received
    if (message.get_code() != 0) {
      std::cerr << "Received ICMP message with invalid code " << message.get_code() << std::endl;
      return;
    }
    // TODO: Do additional check to make sure the length of the ICMP message is no longer than 8 using the packet

    new_message.set_type(9);
    new_message.set_num_addrs(1);
    new_message.set_addr_entry_size(2);
    new_message.add_addr_and_pref(ip_addr, 1);

    new_packet.set_source(ip_addr);
    new_packet.set_destination(0xe0000001);
    new_packet.set_protocol(1);
    new_packet.encapsulate(new_message);

    new_frame.encapsulate(new_packet);
    new_frame.set_source_address(macAddress);
    new_frame.set_destination_address(source_frame.get_source_address());
    new_frame.set_type(0x0800);
    new_frame.get_bit_string(send_buffer);
    send(clientfd, send_buffer, BUFFER_SIZE, 0);

  } else {
    std::cerr << "Received ICMP message with unknown type " << message.get_type() << std::endl;
    return;
  }
}

void Router::process_datagram(Ethernet frame, Datagram datagram) {
  if (datagram.get_destination_port() == 67) {
    DHCP_Message message;
    datagram.unencapsulate_dhcp_message(&message);
    this->dhcp_server->handle_message(frame, message);
  } else {
    std::cerr << "datagram received with unknown destination port" << datagram.get_destination_port() << std::endl;
    return;
  }
}

void Router::handleConnection(int socketfd, Router *router) {
  std::cout << "Handling connection for socket " << socketfd << std::endl;
  char buffer[BUFFER_SIZE];
  Ethernet ethernet_frame;
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

/*
This method is used to send the router's
frame.
*/
void Router::send_frame(Ethernet frame) {
  frame.get_bit_string(send_buffer);
  send(clientfd, send_buffer, BUFFER_SIZE, 0);
}
