#include "../hosts/router.h"
#include "dhcp.h"
#include "datagram.h"
#include "ip.h"
#include "ethernet.h"
#include <iostream>

DHCP_Server::DHCP_Server() {
  for (int i = 0; i < 255; i++) {
    available_ips[i] = 1;
  }
  available_ips[0] = 0;
  available_ips[1] = 0;
}

DHCP_Server::DHCP_Server(Router* router) {
  for (int i = 0; i < 255; i++) {
    available_ips[i] = 1;
  }
  available_ips[0] = 0;
  available_ips[1] = 0;
  this->router = router;
}

void DHCP_Server::handle_message(Ethernet source_frame, DHCP_Message message) {
  if (message.is_broadcast() && message.get_ciaddr() == 0 
      && message.get_giaddr() == 0 && message.get_xid() != last_xid
      && message.option_is_set(53) && message.get_option(53) == 1) {
    // DHCP DISCOVER

    Datagram datagram;
    IP packet;
    Ethernet frame;

    std::cout << "DHCP DISCOVER RECEIVED. DHCP OFFER BEING RETURNED." << std::endl;
    int base_ip = 0b11000000'10101000'00000000'00000000;
    int new_ip = base_ip;

    last_xid = message.get_xid();
    for (int i = 0; i < 255; i++) {
      if (available_ips[i]) {
        new_ip = new_ip | i;
        available_ips[i] = 0;
        break;
      }
    }
    if ((new_ip & 0xff) == 0xff) {
      std::cout << "Could not assign host an IP address" << std::endl;
      return;
    }
    message.set_yiaddr(new_ip);
    message.set_siaddr(this->router->get_ip_addr());
    message.clear_options();
    message.set_option(1, 4, 0xffffff00);
    message.set_option(3, 4, this->router->get_ip_addr());
    message.set_option(53, 1, 2);

    datagram.set_payload(message);

    datagram.set_source_port(67);
    datagram.set_destination_port(68);

    packet.set_payload(datagram);
    packet.set_destination(IP_BROADCAST);
    packet.set_source(this->router->get_ip_addr());
    packet.set_protocol(17);

    frame.set_source_address(this->router->get_mac_address());
    frame.set_destination_address(source_frame.get_source_address());
    frame.encapsulate(packet);
    frame.set_type(0x0800);
    frame.get_bit_string(this->router->send_buffer);
    send(this->router->clientfd, this->router->send_buffer, BUFFER_SIZE, 0);

  } else if (message.is_broadcast() && message.get_xid() == last_xid 
            && message.option_is_set(53) && message.get_option(53) == 3) {
    // DHCP REQUEST

    Datagram datagram;
    IP packet;
    Ethernet frame;

    std::cout << "DHCP REQUEST RECEIVED. DHCP ACK BEING RETURNED." << std::endl;

    message.clear_options();
    message.set_option(1, 4, 0xffffff00);
    message.set_option(3, 4, this->router->get_ip_addr());
    message.set_option(53, 1, 5);

    datagram.set_payload(message);
    datagram.set_source_port(67);
    datagram.set_destination_port(68);

    packet.set_payload(datagram);
    packet.set_destination(message.get_yiaddr());
    packet.set_source(this->router->get_ip_addr());
    packet.set_protocol(17);

    frame.set_source_address(this->router->get_mac_address());
    frame.set_destination_address(source_frame.get_source_address());
    frame.encapsulate(packet);
    frame.set_type(0x0800);
    frame.get_bit_string(this->router->send_buffer);
    send(this->router->clientfd, this->router->send_buffer, BUFFER_SIZE, 0);
  }
}

/*
This is a setter to set the router property.
Parameters:
  router - The router object that this server
  will reference.
*/
void DHCP_Server::set_router(Router* router) {
  this->router = router;
}
// ----------------------- DHCP_Message ---------------------------

DHCP_Message::DHCP_Message() {
  options[0] = 99;
  options[1] = 130;
  options[2] = 83;
  options[3] = 99;
}

void DHCP_Message::set_op(unsigned char operation) {
  op = operation;
}

void DHCP_Message::set_ciaddr(int ciaddr) {
  this->ciaddr = ciaddr;
}

void DHCP_Message::set_yiaddr(int yiaddr) {
  this->yiaddr = yiaddr;
}

void DHCP_Message::set_siaddr(int siaddr) {
  this->siaddr = siaddr;
}

void DHCP_Message::set_giaddr(int giaddr) {
  this->giaddr = giaddr;
}


int DHCP_Message::get_ciaddr() {
  return this->ciaddr;
}

int DHCP_Message::get_yiaddr() {
  return this->yiaddr;
}

int DHCP_Message::get_siaddr() {
  return this->siaddr;
}

int DHCP_Message::get_giaddr() {
  return this->giaddr;
}

void DHCP_Message::set_broadcast() {
  this->flags = this->flags | 1;
}

bool DHCP_Message::is_broadcast() {
  return this->flags & 1;
}

/*
This method is used when encapsulating the DHCP message
in a datagram. It is used to populate the payload of the
datagram.
Parameters:
  buffer - An unsigned char buffer of exactly DHCP_LENGTH
*/
void DHCP_Message::to_bytes(unsigned char* buffer) {
  memset(buffer, 0, DHCP_LENGTH);
  memcpy(buffer, &op, 1);
  buffer += 1;
  memcpy(buffer, &htype, 1);
  buffer += 1;
  memcpy(buffer, &hlen, 1);
  buffer += 1;
  memcpy(buffer, &hops, 1);
  buffer += 1;
  memcpy(buffer, &xid, 4);
  buffer += 4;
  memcpy(buffer, &secs, 2);
  buffer += 2;
  memcpy(buffer, &flags, 2);
  buffer += 2;
  memcpy(buffer, &ciaddr, 4);
  buffer += 4;
  memcpy(buffer, &yiaddr, 4);
  buffer += 4;
  memcpy(buffer, &siaddr, 4);
  buffer += 4;
  memcpy(buffer, &giaddr, 4);
  buffer += 4;
  memcpy(buffer, chaddr, 16);
  buffer += 16;
  memcpy(buffer, sname, 64);
  buffer += 64;
  memcpy(buffer, file, 128);
  buffer += 128;
  memcpy(buffer, options, DHCP_OPTIONS_LENGTH);
  buffer += DHCP_OPTIONS_LENGTH;
}

/*
This method is used when a datagram is received that contains
a DHCP message and we want to initialize the dhcp message from
the datagram payload bytes.
Parameters:
  buffer - A buffer of exactly DATAGRAM_PAYLOAD_LENGTH
*/
void DHCP_Message::initialize_from_bytes(unsigned char* buffer) {
  memcpy(&op, buffer, 1);
  buffer += 1;
  memcpy(&htype, buffer, 1);
  buffer += 1;
  memcpy(&hlen, buffer, 1);
  buffer += 1;
  memcpy(&hops, buffer, 1);
  buffer += 1;
  memcpy(&xid, buffer, 4);
  buffer += 4;
  memcpy(&secs, buffer, 2);
  buffer += 2;
  memcpy(&flags, buffer, 2);
  buffer += 2;
  memcpy(&ciaddr, buffer, 4);
  buffer += 4;
  memcpy(&yiaddr, buffer, 4);
  buffer += 4;
  memcpy(&siaddr, buffer, 4);
  buffer += 4;
  memcpy(&giaddr, buffer, 4);
  buffer += 4;
  memcpy(chaddr, buffer, 16);
  buffer += 16;
  memcpy(sname, buffer, 64);
  buffer += 64;
  memcpy(file, buffer, 128);
  buffer += 128;
  memcpy(options, buffer, DHCP_OPTIONS_LENGTH);
}

/*
When a message is received, the options need to be cleared before new options can be set.
*/
void DHCP_Message::clear_options() {
  memset(options, 0, DHCP_OPTIONS_LENGTH);
  option_index = 0;
}
/*
This method is used for setting various options on a dhcp message.
Parameters:
  code - The option code.
  length - The length of the option data.
  data - A buffer with the option data.
*/
void DHCP_Message::set_option(unsigned char code, unsigned char length, unsigned char* data) {
  options[option_index++] = code;
  options[option_index++] = length;
  for (int i = 0; i < length; i++) {
    options[option_index++] = data[i];
  }
}

/*
This method is used for setting various options on a dhcp message.
Parameters:
  code - The option code.
  length - The length of the option data.
  data - An int with the option data.
*/
void DHCP_Message::set_option(int code, int length, int data) {
  options[option_index++] = code;
  options[option_index++] = length;
  memcpy(options + option_index, &data, length);
  option_index += length;
}

/*
This method is used to check if an option is set.
Sometimes we must just check for the presence of
an option and the actual value is not relevant
at that point in time.
Parameters:
  opcode - The option code to check for the existence of.
*/
bool DHCP_Message::option_is_set(int opcode) {
  int index = 0;
  unsigned char current_code = 0;
  while (index < DHCP_OPTIONS_LENGTH) {
    current_code = options[index];
    if (current_code == opcode) {
      return true;
    }
    index++;
    index += options[index] + 1;
  }
  return false;
}

int DHCP_Message::get_option(int opcode) {
  option_index = 0;
  int returnVal = 0;
  while (option_index < DHCP_OPTIONS_LENGTH) {
    if (options[option_index++] == opcode) {
      if (options[option_index] > 4 || options[option_index] < 1) {
        std::cerr << "Found the specified option, but the length of the data was " << options[option_index] << std::endl;
        return 0;
      }
      memcpy(&returnVal, options+option_index+1, options[option_index]);
      return returnVal;
    }
    option_index += options[option_index] + 1;
  }
  return 0;
}