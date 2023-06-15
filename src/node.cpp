
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

void Node::obtain_ip_address() {

  dhcp_message.set_op(1);
  dhcp_message.set_ciaddr(0);
  dhcp_message.set_giaddr(0);
  dhcp_message.set_broadcast();

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
  
  send(sockfd, send_buffer, BUFFER_SIZE, 0);
  read(sockfd, recv_buffer, BUFFER_SIZE);

  frame.load_frame_from_string(recv_buffer);
  if (frame.get_destination_address() == 0x00ffffffffffff) {
    std::cout << "Received a reply frame!" << std::endl;
  } else {
    std::cout << "Received a non reply frame..." << std::endl;
  }
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
