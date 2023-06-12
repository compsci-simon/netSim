#include "dhcp.h"
#include <iostream>

DHCP_Server::DHCP_Server(Router* router) {
  for (int i = 0; i < 255; i++) {
    available_ips[i] = 1;
  }
  available_ips[0] = 0;
  available_ips[1] = 0;
  this->router = router;
}

void DHCP_Server::handle_dhcp_message(DHCP_Message* message) {

  if (message->is_broadcast() && message->get_ciaddr() == 0 && message->get_giaddr() == 0) {
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
      message->set_yiaddr(new_ip);
      message->set_siaddr(base_ip | 1);
    }
  }
}

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
