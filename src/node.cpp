#include "logging.h"
#include "node.h"
#include "utils.h"
#include "arp.h"
#include <chrono>

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

void Node::disconnect() {
  receive_thread.join();
  close(sockfd);
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
  while (listen) {
    memset(recv_buffer, 0, BUFFER_SIZE);
    bytes_read = read(sockfd, recv_buffer, BUFFER_SIZE);
    if (bytes_read <= 0) {
      listen = false;
      std::cerr << "Connection broken" << std::endl;
      break;
    }

    frame.instantiate_from_bit_string(recv_buffer);
    if (frame.get_destination_address() == 0x00ffffffffffff ||
      frame.get_destination_address() == this->macAddress) {
      if (frame.get_type() == 0x0800) {
        // IP Packet
        frame.load_packet(&packet);
        handle_packet(packet);
      } else if (frame.get_type() == 0x0806) {
        // ARP Query
      } else {
        std::cerr << "Unknown frame protocol type: " << frame.get_type_string() << std::endl;
      }
    } else {
      std::cout << "Received a frame but discared the frame as it's destination address is not our macAddress or the broadcast address. Frame address = " << frame.address_to_string(false) << std::endl;
      return;
    }
  }
}

/*
This method is used for processing packets.
Parameters:
  packet - The packet to process.
*/
void Node::handle_packet(Packet packet) {
  if (packet.get_destination() == 0xffffffff 
    || packet.get_destination() == this->ipAddress) {
    
    if (packet.get_protocol() == 17) {
      packet.load_datagram(&datagram);
      handle_datagram(datagram);
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
    Frame frame;
    query.set_target_hardware(this->macAddress);
    frame.set_destination(query.get_source_hardware());
    frame.set_source(this->macAddress);
    frame.set_type(0x0806);
    frame.multiplex(query);
    frame.get_bit_string(send_buffer);
    send(sockfd, send_buffer, BUFFER_SIZE, 0);
  } else {
    // Silently dismiss ARP query
    char buf[17] {0};
    Packet::address_to_string(query.get_target_protocol(), buf);
    std::cout << "Received ARP query for " << buf << std::endl;
  }
}

/*
This method is used for processing datagrams.
Parameters:
  datagram - The datagrama to process.
*/
void Node::handle_datagram(Datagram datagram) {
  if (datagram.get_destination_port() == 68) {
    datagram.unencapsulate_dhcp_message(&dhcp_message);
    process_dhcp_message(dhcp_message);
  } else {
    std::cerr << "Datagram discarded with unknown destination port " << (int) datagram.get_destination_port() << std::endl;
  }
}

/*
This method processes dhcp messages.
Parameters:
  message - The DHCP message to process.
*/
void Node::process_dhcp_message(DHCP_Message message) {
  if (!dhcp_message.option_is_set(53)) {
    std::cerr << "DHCP message received without option 53 being set." << std::endl;
    return;
  }
  if (dhcp_message.get_option(53) == 2) {
    // DHCP OFFER
    if (dhcp_message.get_xid() != 1234567) {
      std::cerr << "DHCP Message with incorrect xid received." << std::endl;
      return;
    }
    this->set_ip_address(dhcp_message.get_yiaddr());
    this->dhcp_request(dhcp_message);
  } else if (dhcp_message.get_option(53) == 4) {
    // DHCP NAK
  } else if (dhcp_message.get_option(53) == 5) {
    // DHCP ACK
    this->dhcp_bind(dhcp_message);
  }
}

void Node::dhcp_discover() {

  // DHCP DISCOVER
  dhcp_message.set_op(1);
  dhcp_message.set_option(53, 1, 1);
  dhcp_message.set_ciaddr(0);
  dhcp_message.set_giaddr(0);
  dhcp_message.set_broadcast();
  dhcp_message.set_xid(1234567);

  datagram.set_source_port(68);
  datagram.set_destination_port(67);
  datagram.set_payload(dhcp_message);

  packet.set_destination(0xffffffff);
  packet.set_source(0);
  packet.set_payload(datagram);
  packet.set_options(1234);
  packet.set_protocol(17);

  frame.set_source(macAddress);
  frame.set_destination(0x00ffffffffffff);
  frame.set_payload(packet);
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
void Node::dhcp_request(DHCP_Message message) {
  std::cout << "IN REQUESTING STATE" << std::endl;
  message.clear_options();
  message.set_option(53, 1, 3);
  datagram.set_source_port(68);
  datagram.set_destination_port(67);
  datagram.set_payload(message);

  packet.set_source(0);
  packet.set_destination(message.get_siaddr());
  packet.set_payload(datagram);
  packet.set_protocol(17);

  frame.swap_source_and_dest();
  frame.set_payload(packet);
  frame.get_bit_string(send_buffer);
  send(sockfd, send_buffer, BUFFER_SIZE, 0);
}

void Node::dhcp_bind(DHCP_Message message) {
  char buffer[17] {0};
  Packet::address_to_string(this->ipAddress, buffer);
  std::cout << "BINDING. PERFORMING ARP QUERY..." << std::endl;
  Arp query;
  query.set_operation(1);
  query.set_source_hardware(this->macAddress);
  query.set_source_protocol(this->ipAddress);
  query.set_target_hardware(0);
  query.set_target_protocol(this->ipAddress);

  frame.multiplex(query);
  frame.set_destination(0x00ffffffffffff);
  frame.set_source(macAddress);
  frame.set_type(0x0806);
  frame.get_bit_string(send_buffer);
  send(sockfd, send_buffer, BUFFER_SIZE, 0);

  std::this_thread::sleep_for(std::chrono::seconds(3));
  std::cout << "IN BINDING STATE. IP address accepted: " << buffer << std::endl;
  listen = false;
}
