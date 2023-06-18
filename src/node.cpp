
#include "logging.h"
#include "node.h"
#include "utils.h"

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

  frame.swap_source_and_dest();
  frame.set_payload(packet);
  frame.get_bit_string(send_buffer);
  send(sockfd, send_buffer, BUFFER_SIZE, 0);
}

void Node::dhcp_bind(DHCP_Message message) {
  char buffer[17] {0};
  Packet::address_to_string(this->ipAddress, buffer);
  std::cout << "IN BINDING STATE. IP address accepted: " << buffer << std::endl;
  listen = false;
}

/*
This method is used for address resolution, i.e.
to determine what mac address is assosciated with a given 
IP address.
*/
void Node::arp(int address) {

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
      frame.load_packet(&packet);
      if (packet.get_destination() == 0xffffffff 
        || packet.get_destination() == this->ipAddress ) {
        packet.load_datagram(&datagram);
        if (datagram.get_destination_port() == 68) {
          datagram.unencapsulate_dhcp_message(&dhcp_message);
          if (dhcp_message.get_xid() == 1234567 
              && dhcp_message.is_broadcast() && dhcp_message.option_is_set(53)) {
            if (dhcp_message.get_option(53) == 2) {
              // DHCP Offer
              this->set_ip_address(dhcp_message.get_yiaddr());
              this->dhcp_request(dhcp_message);
            } else if (dhcp_message.get_option(53) == 5) {
              // DHCP ACK
              this->dhcp_bind(dhcp_message);
            }
          } else {
            std::cerr << "dhcp_message.option_is_set(53) = " << dhcp_message.option_is_set(53) << std::endl;
            std::cerr << "dhcp_message.get_option(53) = " << dhcp_message.get_option(53) << std::endl;
            listen = false;
            return;
          }
        }
      }
    } else {
      std::cout << "Received a frame but discared the frame as it's destination address is not our macAddress or the broadcast address. Frame address = " << frame.address_to_string(false) << std::endl;
      return;
    }
  }
}