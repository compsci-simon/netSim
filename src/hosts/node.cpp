#include <chrono>
#include <random>

#include "node.h"
#include "../utils/logging.h"
#include "../utils/utils.h"
#include "../protocols/ethernet.h"
#include "../protocols/ip.h"
#include "../protocols/arp.h"
#include "../protocols/icmp.h"
#include "../protocols/datagram.h"
#include "../protocols/dhcp.h"
#include "../utils/logging.h"

Node::Node (int port, char *host, const char* name) {
  this->port = port;
  this->host = host;
  this->name = name;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(0, 15);
  macAddress = 0;
  for (int i = 0; i < 12; i++) {
    macAddress = macAddress << 8;
    macAddress += dis(gen);
  }
  macAddress = 0x123456789abc;
  logger = new Logger(name);
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
  serverAddress.sin_port = htons(1234);
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

void Node::disconnect() {
  listen = false;
  close(sockfd);
  receive_thread.join();
}

/*
This method is used to start a thread that will listen
for and process incoming frames.
*/
void Node::start_listen_thread() {
  receive_thread = std::thread(&Node::handle_frame, this);
}

/*
This method is started in a child thread. The
sole responsibility of this method is to load an incoming
frame from the recv_buffer and decide how to handle the frame.
*/
void Node::handle_frame() {
  int bytes_read = 0;
  listen = true;
  Ethernet frame;
  while (listen) {
    memset(recv_buffer, 0, BUFFER_SIZE);
    bytes_read = read(sockfd, recv_buffer, BUFFER_SIZE);
    if (bytes_read <= 0) {
      listen = false;
      std::cerr << "Connection broken" << std::endl;
      break;
    }

    frame.instantiate_from_bit_string(recv_buffer);
    if (frame.get_destination_address() == 0x00ffffffffffff 
        || frame.get_destination_address() == this->macAddress) {

      if (frame.get_type() == 0x0800) {
        // IP packet
        IP packet;
        logger->log("Received IP packet");
        frame.decapsulate(&packet);
        handle_packet(frame, packet);
      } else if (frame.get_type() == 0x0806) {
        // ARP Query
        Arp query;
        std::cout << "Received ARP packet" << std::endl;
        frame.decapsulate(&query);
        process_arp(query);
      } else {
        std::cerr << "Unknown frame protocol type: " << frame.get_type_string() << std::endl;
      }
    } else {
      std::string s = "Received a frame but discared the frame as it's destination address is not our macAddress or the broadcast address. Ethernet address = ";
      s.append((const char*)frame.address_to_string(false));
      logger->log(s.c_str());
      return;
    }
  }
}

/*
This method is used for processing packets.
Parameters:
  packet - The packet to process.
*/
void Node::handle_packet(Ethernet frame, IP packet) {
  if (packet.get_destination() == 0xffffffff 
    || packet.get_destination() == this->ipAddress
    || packet.get_destination() == 0xe0000001) {
    
    if (packet.get_protocol() == 1) {
      // ICMP message
      ICMP icmp_message;
      packet.unencapsulate(&icmp_message);
      process_icmp_packets(icmp_message);
    } else if (packet.get_protocol() == 17) {
      Datagram datagram;
      packet.load_datagram(&datagram);
      handle_datagram(frame, datagram);
    } else {
      std::cerr << "Discarded packet with protocol " << ((int) packet.get_protocol()) << std::endl;
    }
  } else {
    std::cerr << "Discarded a packet with protocol address " << packet.address_to_string(false) << std::endl;
  }
}

void Node::process_arp(Arp query) {
  if (query.get_target_protocol() == this->ipAddress) {
    // Respond to ARP query
    Ethernet frame;
    query.set_target_hardware(this->macAddress);
    frame.set_destination_address(query.get_source_hardware());
    frame.set_source_address(this->macAddress);
    frame.set_type(0x0806);
    frame.encapsulate(query);
    frame.get_bit_string(send_buffer);
    send(sockfd, send_buffer, BUFFER_SIZE, 0);
  } else if (query.get_source_protocol() == this->ipAddress) {
    if (query.get_target_hardware() == 0) {
      logger->log("Dismissing arp query with target hardware set to 0");
      return;
    }
    std::vector<long> pair {query.get_target_protocol(), query.get_target_hardware()};
    arp_table.push_back(pair);
  } else {
    // Silently dismiss ARP query
    char buf[17] {0};
    IP::address_to_string(query.get_target_protocol(), buf);
    std::string s = "Received ARP query for target ";
    s.append(buf);
    logger->log(s.c_str());
    IP::address_to_string(query.get_source_protocol(), buf);
    s = "Received ARP query for source ";
    s.append(buf);
    logger->log(s.c_str());
  }
}

void Node::arp_query(int target_addr) {
  Ethernet frame;
  Arp query;

  query.set_source_hardware(macAddress);
  query.set_source_protocol(ipAddress);
  query.set_target_protocol(target_addr);

  frame.encapsulate(query);
  frame.set_destination_address(0xffffffffffff);
  frame.set_source_address(ipAddress);
  frame.set_type(0x0806);
  frame.get_bit_string(send_buffer);

  send(sockfd, send_buffer, BUFFER_SIZE, 0);

}

void Node::process_icmp_packets(ICMP packet) {
  if (packet.get_type() == 0) {
    // Echo reply
    std::cout << "Received echo reply" << std::endl;
    return;
  } if (packet.get_type() == 9) {
    // Router advertisement
    if (packet.get_code() == 0) {
      std::cerr << "Received ICMP router advertisement with non-zero code " << packet.get_code() << std::endl;
      return;
    }
    if (packet.get_num_addrs() < 1) {
      std::cerr << "Received ICMP router advertisement with num addrs less than 1 " << packet.get_num_addrs() << std::endl;
      return;
    }
    if (packet.get_addr_entry_size() < 2) {
      std::cerr << "Received ICMP router advertisement with addr entry size less than 2 " << packet.get_addr_entry_size() << std::endl;
      return;
    }
    if (perform_router_discover) {
      set_router_ip(packet.get_addr(0));
    } else {
      std::cerr << "Discarded ICMP message due to router discovery being turned off" << std::endl;
      return;
    }
  } else {
    std::cerr << "Received ICMP message with unknown type " << (int) packet.get_type() << std::endl;
    return;
  }
}

/*
This method is used for processing datagrams.
Parameters:
  datagram - The datagrama to process.
*/
void Node::handle_datagram(Ethernet frame, Datagram datagram) {
  if (datagram.get_destination_port() == 68) {
    DHCP_Message message;
    datagram.unencapsulate_dhcp_message(&message);
    process_dhcp_message(frame, message);
  } else {
    std::cerr << "Datagram discarded with unknown destination port " << (int) datagram.get_destination_port() << std::endl;
  }
}

/*
This method processes dhcp messages.
Parameters:
  message - The DHCP message to process.
*/
void Node::process_dhcp_message(Ethernet frame, DHCP_Message message) {
  if (!message.option_is_set(53)) {
    std::cerr << "DHCP message received without option 53 being set." << std::endl;
    return;
  }
  if (message.get_option(53) == 2) {
    // DHCP OFFER
    if (message.get_xid() != 1234567) {
      std::cerr << "DHCP Message with incorrect xid received." << std::endl;
      return;
    }
    this->set_ip_address(message.get_yiaddr());
    this->set_subnet_mask(message.get_option(1));
    this->set_router_ip(message.get_option(3));
    this->dhcp_request(frame, message);
  } else if (message.get_option(53) == 4) {
    // DHCP NAK
  } else if (message.get_option(53) == 5) {
    // DHCP ACK
    this->dhcp_bind(message);
  }
}

void Node::dhcp_discover() {
  DHCP_Message message;
  Datagram datagram;
  IP packet;
  Ethernet frame;
  // DHCP DISCOVER
  message.set_op(1);
  message.set_option(53, 1, 1);
  message.set_ciaddr(0);
  message.set_giaddr(0);
  message.set_broadcast();
  message.set_xid(1234567);

  datagram.set_source_port(68);
  datagram.set_destination_port(67);
  datagram.set_payload(message);

  packet.set_destination(0xffffffff);
  packet.set_source(0);
  packet.set_payload(datagram);
  packet.set_options(1234);
  packet.set_protocol(17);

  frame.set_source_address(macAddress);
  frame.set_destination_address(0x00ffffffffffff);
  frame.encapsulate(packet);
  frame.get_bit_string(send_buffer);
  
  send(sockfd, send_buffer, BUFFER_SIZE, 0);
}

/*
If a dhcp discover receives a reply, then the process
can continue to a dhcp request from the client to the
server.
Parameters:
  message - The DHCP Message received from the server.
*/
void Node::dhcp_request(Ethernet source_frame, DHCP_Message message) {
  Datagram datagram;
  IP packet;
  Ethernet frame;

  logger->log("IN REQUESTING STATE");
  message.clear_options();
  message.set_option(53, 1, 3);
  datagram.set_source_port(68);
  datagram.set_destination_port(67);
  datagram.set_payload(message);

  packet.set_source(0);
  packet.set_destination(message.get_siaddr());
  packet.set_payload(datagram);
  packet.set_protocol(17);

  frame.encapsulate(packet);
  frame.set_source_address(macAddress);
  frame.set_destination_address(source_frame.get_source_address());
  frame.set_type(0x0800);
  frame.get_bit_string(send_buffer);
  send(sockfd, send_buffer, BUFFER_SIZE, 0);
}

void Node::dhcp_bind(DHCP_Message message) {
  Ethernet frame;
  Arp query;
  char buffer[17] {0};

  std::cout << "BINDING. PERFORMING ARP QUERY..." << std::endl;
  
  query.set_operation(1);
  query.set_source_hardware(this->macAddress);
  query.set_source_protocol(this->ipAddress);
  query.set_target_hardware(0);
  query.set_target_protocol(this->ipAddress);

  frame.encapsulate(query);
  frame.set_destination_address(0x00ffffffffffff);
  frame.set_source_address(macAddress);
  frame.set_type(0x0806);
  frame.get_bit_string(send_buffer);
  send(sockfd, send_buffer, BUFFER_SIZE, 0);

  std::this_thread::sleep_for(std::chrono::seconds(3));
  IP::address_to_string(this->ipAddress, buffer);
  std::string s = "IP address accepted: ";
  s.append(buffer);
  logger->log(s.c_str());
  IP::address_to_string(this->subnet_mask, buffer);
  s = "Subnet mask: ";
  s.append(buffer);
  logger->log(s.c_str());
  IP::address_to_string(this->router_ip, buffer);
  s = "Gatewat IP: ";
  s.append(buffer);
  logger->log(s.c_str());
}

void Node::router_solicitation() {
  Ethernet frame;
  IP ip_packet;
  ICMP icmp_packet;

  icmp_packet.set_type(10);
  icmp_packet.set_code(0);

  ip_packet.set_destination(0xe0000001);
  ip_packet.set_source(ipAddress);
  ip_packet.set_protocol(1);
  ip_packet.encapsulate(icmp_packet);

  frame.encapsulate(ip_packet);
  frame.set_source_address(macAddress);
  frame.set_destination_address(0x00ffffffffffff);
  frame.get_bit_string(send_buffer);
  send(sockfd, send_buffer, BUFFER_SIZE, 0);

}

void Node::ping(int target) {
  Ethernet frame;
  IP packet;
  ICMP message;
  long int mac = 0;
  char buf[17] {0};

  IP::address_to_string(target, buf);

  for (int attempt = 0; attempt < 3; attempt++) {
    for (int i = 0; i < arp_table.size(); i++) {
      if (arp_table.at(i).at(0) == target) {
        mac = arp_table.at(i).at(1);
      }
    }
    if (mac != 0) {
      break;
    }
    arp_query(target);
    std::string s = "Sending ARP query to ";
    s.append(buf);
    logger->log(s.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  if (mac == 0) {
    std::cerr << "Could not obtain mac address for " << buf << std::endl;
    return;
  }

  message.set_code(0);
  message.set_type(8);

  packet.encapsulate(message);
  packet.set_destination(target);
  packet.set_source(ipAddress);
  packet.set_protocol(1);

  frame.encapsulate(packet);
  frame.set_destination_address(mac);
  frame.set_source_address(macAddress);
  frame.set_type(0x0800);
  frame.get_bit_string(send_buffer);
  send(sockfd, send_buffer, BUFFER_SIZE, 0);
}
