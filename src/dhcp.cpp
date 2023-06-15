#include "router.h"
#include <iostream>

DHCP_Server::DHCP_Server() {}

DHCP_Server::DHCP_Server(Router* router) {
  for (int i = 0; i < 255; i++) {
    available_ips[i] = 1;
  }
  available_ips[0] = 0;
  available_ips[1] = 0;
  this->router = router;
}

void DHCP_Server::handle_message(DHCP_Message message) {
  if (message.is_broadcast() && message.get_ciaddr() == 0 && message.get_giaddr() == 0) {
    Router* r;
    /*
    If these conditions are met it means the message is a dhcp discover message for a client
    trying to obtain an IP address.
    */
    // 192.168.0.0
    int base_ip = 0b11000000'10100100'00000000'00000000;
    int new_ip = base_ip;
    for (int i = 0; i < 255; i++) {
      if (available_ips[i]) {
        new_ip = new_ip | i;
        available_ips[i] = 0;
        break;
      }
      if (i == 256) {
        std::cout << "Could not assign host an IP address" << std::endl;
        return;
      }
      message.set_yiaddr(new_ip);
      message.set_ciaddr(this->router->get_ip_addr());
    }
    std::cout << "DHCP DISCOVER RECEIVED. DHCP OFFER BEING RETURNED." << std::endl;
    r = this->router;
    r->datagram.set_payload(message);
    r->datagram.set_source_port(67);
    r->datagram.set_destination_port(68);

    r->packet.set_payload(r->datagram);
    r->packet.set_destination(IP_BROADCAST);
    r->packet.set_source(r->get_ip_addr());

    r->frame.set_payload(r->packet);
    r->frame.swap_source_and_dest();
    r->set_self_as_frame_source();
    r->send_frame();
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

