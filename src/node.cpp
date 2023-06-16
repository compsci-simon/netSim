
#include "logging.h"
#include "node.h"
#include "utils.h"

int Node::sendMessageToServer(std::string message) {
  return 0;
}

void Node::receive_messages_from_server() {
  memset(recv_buffer, 0, 1526);
  if (read(sockfd, recv_buffer, BUFFER_SIZE) <= 0) {
    listen = false;
  }
}

void Node::echo_messages_received() {
  while (listen) {
    receive_messages_from_server();
    std::cout << recv_buffer << std::endl;
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
  dhcp_message.set_ciaddr(0);
  dhcp_message.set_giaddr(0);
  dhcp_message.set_broadcast();
  dhcp_message.set_xid(1234567);

  datagram.set_source_port(68);
  datagram.set_destination_port(67);
  datagram.set_payload(dhcp_message);

  packet.set_destination(IP_BROADCAST);
  packet.set_source(0);
  packet.set_payload(datagram);

  frame.set_source(macAddress);
  frame.set_destination(0x00ffffffffffff);
  frame.set_payload(packet);
  frame.get_byte_string(send_buffer);
  
  send(sockfd, send_buffer, 1526, 0);
  read(sockfd, recv_buffer, 1526);

  frame.load_frame_from_string(recv_buffer);
  if (frame.get_destination_address() == 0x00ffffffffffff) {
    frame.load_packet(&packet);
    if (packet.get_destination() == 0xffffffff) {
      packet.load_datagram(&datagram);
      if (datagram.get_destination_port() == 68) {
        datagram.unencapsulate_dhcp_message(&dhcp_message);
        if (dhcp_message.get_xid() == 1234567 
            && dhcp_message.is_broadcast() && dhcp_message.option_is_set(53)) {
          this->set_ip_address(dhcp_message.get_yiaddr());
          this->dhcp_request(dhcp_message);
        }
      }
    }
  } else {
    std::cout << "Received a non reply frame..." << std::endl;
    return;
  }
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
  
  datagram.set_source_port(68);
  datagram.set_destination_port(67);
  datagram.set_payload(message);

  packet.set_source(0);
  packet.set_destination(message.get_siaddr());
  packet.set_payload(datagram);

  frame.swap_source_and_dest();
  frame.set_payload(packet);
  frame.get_byte_string(send_buffer);
  send(sockfd, send_buffer, 1526, 0);

  memset(recv_buffer, 0, 1526);
  read(sockfd, recv_buffer, BUFFER_SIZE);
  frame.load_frame_from_string(recv_buffer);
  if (frame.get_destination_address() == 0x00ffffffffffff 
      || frame.get_destination_address() == this->macAddress) {
    frame.load_packet(&packet);
    if (packet.get_destination() == this->ipAddress) {
      packet.load_datagram(&datagram);
      if (datagram.get_destination_port() == 68) {
        datagram.unencapsulate_dhcp_message(&dhcp_message);
        if (dhcp_message.get_xid() == 1234567 
            && dhcp_message.is_broadcast() && dhcp_message.option_is_set(53)) {
          this->dhcp_bind(dhcp_message);
        }
      }
    }
  }
  // this->dhcp_bind();
}

void Node::dhcp_bind(DHCP_Message message) {
  unsigned char octets[4] {
    (unsigned char) ((this->ipAddress >> 24) & 0xff),
    (unsigned char) ((this->ipAddress >> 16) & 0xff),
    (unsigned char) ((this->ipAddress >> 8) & 0xff),
    (unsigned char) ((this->ipAddress) & 0xff),
  };
  std::cout << "IN BINDING STATE. IP address accepted: " << (int) octets[0] << "." << (int) octets[1] << "." << (int) octets[2] << "." << (int) octets[3] << std::endl;
}

void Node::disconnect() {
  close(sockfd);
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
