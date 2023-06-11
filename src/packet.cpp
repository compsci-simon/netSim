#include <iostream>
#include <string>
#include "packet.h"

Packet::Packet() {
  memset(data, 0, PACKET_PAYLOAD_SIZE);
}

void Packet::set_destination(const char* address) {
  destination_address = 0;
  int p1 = 0;
  int p2 = 0;
  int count = 0;
  int octet = 3;
  char temp_str[4];

  for (int i = 0; i < 15 && address[i] != '\0'; i++) {
    if (address[i] == '.') count++;
  }
  if (count != 3) {
    std::cerr << "Error decoding address. 3 octects are required. Found " << count << std::endl;
    return;
  }
  for (; octet > 0; octet--) {
    for (; p2 < 15 && address[p2] != '\0'; p2++) {
      temp_str[p2 - p1] = (char) address[p2];
      if (address[p2] == '.') {
        temp_str[1 + p2 - p1] = '\0';
        break;
      }
    }
    p2 = p1;
    destination_address = destination_address << octet*8;
    destination_address = destination_address & atoi(temp_str);
  }
  for (; p2 - p1 < 4 && address[p2] != '\0'; p2++) {
    temp_str[p2 - p1] = address[p2];
  }
  destination_address = destination_address << octet*8;
  destination_address = destination_address & atoi(temp_str);
  std::cout << destination_address << std::endl;
}

void Packet::set_destination(int address) {
  destination_address = address;
}

void Packet::set_source(int address) {
  source_address = address;
}

/*
This function is used to set the payload of an IP packet.
Parameters:
  buffer - A 1476 byte buffer that will be copied to the packet's
    payload
*/
void Packet::set_payload(unsigned char* buffer) {
  memset(data, 0, PACKET_PAYLOAD_SIZE);
  memcpy(data, buffer, PACKET_PAYLOAD_SIZE);
}

/*
This function is used for transferring the payload of the
packet into a buffer that is passed in.
Parameters:
  buffer - This buffer should be exactly PACKET_PAYLOAD_SIZE bytes
*/
void Packet::get_payload(unsigned char* buffer) {
  memset(buffer, 0, PACKET_PAYLOAD_SIZE);
  memcpy(buffer, data, PACKET_PAYLOAD_SIZE);
}

/*
This function copies a byte string representation of this packet into a buffer.
Parameters:
  buffer - A buffer that is of size PACKET_SIZE that will hold all the information.
*/
void Packet::to_byte_string(unsigned char* buffer) {
  memset(buffer, 0, PACKET_SIZE);
  memcpy(buffer, &V_IHL, 1);
  buffer += 1;
  memcpy(buffer, &TOS, 1);
  buffer += 1;
  memcpy(buffer, &total_length, 2);
  buffer += 2;
  memcpy(buffer, &identification, 2);
  buffer += 2;
  memcpy(buffer, &flags_and_offset, 2);
  buffer += 2;
  memcpy(buffer, &TTL, 1);
  buffer += 1;
  memcpy(buffer, &protocol, 1);
  buffer += 1;
  memcpy(buffer, &header_checksum, 2);
  buffer += 2;
  memcpy(buffer, &source_address, 4);
  buffer += 4;
  memcpy(buffer, &destination_address, 4);
  buffer += 4;
  memcpy(buffer, &options, 8);
  buffer += 8;
  memcpy(buffer, data, PACKET_PAYLOAD_SIZE);
}

void Packet::load_packet_from_byte_string(unsigned char* byte_string) {
  memcpy(&V_IHL, byte_string, 1);
  byte_string += 1;
  memcpy(&TOS, byte_string, 1);
  byte_string += 1;
  memcpy(&total_length, byte_string, 2);
  byte_string += 2;
  memcpy(&identification, byte_string, 2);
  byte_string += 2;
  memcpy(&flags_and_offset, byte_string, 2);
  byte_string += 2;
  memcpy(&TTL, byte_string, 1);
  byte_string += 1;
  memcpy(&protocol, byte_string, 1);
  byte_string += 1;
  memcpy(&header_checksum, byte_string, 2);
  byte_string += 2;
  memcpy(&source_address, byte_string, 4);
  byte_string += 4;
  memcpy(&destination_address, byte_string, 4);
  byte_string += 4;
  memcpy(&options, byte_string, 8);
  byte_string += 8;
  memcpy(data, byte_string, PACKET_PAYLOAD_SIZE);
}