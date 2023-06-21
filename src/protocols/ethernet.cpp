#include <iostream>
#include "ethernet.h"
#include "ip.h"
#include "arp.h"
#include "../utils/utils.h"

Ethernet::Ethernet(unsigned char* frame_string) {
  load_frame_from_string(frame_string);
}

void Ethernet::encapsulate(IP packet) {
  packet.to_byte_string(payload);
  type = 0x0800;
}

void Ethernet::decapsulate(IP* packet) {
  packet->load_packet_from_byte_string(payload);
}

/*
Used to multiplex an arp query.
Parameters:
  query - The ARP query to multiplex.
*/
void Ethernet::encapsulate(Arp query) {
  query.get_byte_string(payload);
  type = 0x0806;
}

void Ethernet::decapsulate(Arp* query) {
  query->instantiate_from_byte_string(payload);
}

void Ethernet::get_bit_string(unsigned char* buffer) {
  bytes_to_bits(buffer, preamble, 7);
  buffer += 7*9;
  bytes_to_bits(buffer, &SFD, 1);
  buffer += 1*9;
  bytes_to_bits(buffer, source_address, 6);
  buffer += 6*9;
  bytes_to_bits(buffer, destination_address, 6);
  buffer += 6*9;
  bytes_to_bits(buffer, type, 2);
  buffer += 2*9;
  bytes_to_bits(buffer, payload, 1500);
  buffer += 1500*9;
  bytes_to_bits(buffer, CRC, 4);
}

void Ethernet::instantiate_from_bit_string(unsigned char* buffer) {
  bits_to_bytes(preamble, buffer, 7);
  buffer += 7*9;
  bits_to_bytes(&SFD, buffer, 1);
  buffer += 1*9;
  bits_to_bytes(&source_address, buffer, 6);
  buffer += 6*9;
  bits_to_bytes(&destination_address, buffer, 6);
  buffer += 6*9;
  bits_to_bytes(&type, buffer);
  buffer += 2*9;
  bits_to_bytes(payload, buffer, 1500);
  buffer += 1500*9;
  bits_to_bytes(&CRC, buffer, 4);
}

void Ethernet::load_frame_from_string(unsigned char* frame_string) {
  memcpy(preamble, frame_string, 7);
  frame_string += 7;
  memcpy(&SFD, frame_string, 1);
  frame_string += 1;
  memcpy(&source_address, frame_string, 6);
  frame_string += 6;
  memcpy(&destination_address, frame_string, 6);
  frame_string += 6;
  memcpy(&type, frame_string, 2);
  frame_string += 2;
  memcpy(payload, frame_string, 1500);
  frame_string += 1500;
  memcpy(&CRC, frame_string, 4);
}

/*
This method is used to convert the 48-bit mac address
into a string that is user friendly.
Parameters:
  source - a boolean that determines whether or not to 
    return the source or destination address.
*/
unsigned char* Ethernet::address_to_string(bool source) {
  long* addr;
  int pos = 0;
  unsigned char octetVal;

  memset(address_string, 0, 18);
  if (source) {
    addr = &source_address;
  } else {
    addr = &destination_address;
  }
  for (int byte = 0; byte < 6; byte++) {
    octetVal = (unsigned char) (((*addr) >> ((5 - byte)*8)) & 0xff);
    if (byte != 0) {
      address_string[pos++] = ':';
    }
    byte_to_hex(address_string+(byte*3), octetVal);
    pos += 2;
  }
  return address_string;
}

/*
This method is used to get a string representation
of a frame's type.
*/
unsigned char* Ethernet::get_type_string() {
  memset(address_string, 0, 6);
  address_string[0] = '0';
  address_string[1] = 'x';
  unsigned char val = 0;
  for (int i = 0; i < 4; i++) {
    val = (type >> ((3 - i)*8)) & 0xf;
    if (val < 9) {
      address_string[2 + i] = val + '0';
    } else {
      address_string[2 + i] = val - 10 + 'a';
    }
  }
  return address_string;
}
