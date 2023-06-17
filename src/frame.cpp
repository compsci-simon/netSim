#include <iostream>
#include "frame.h"
#include "packet.h"
#include "utils.h"

Frame::Frame(unsigned char* frame_string) {
  load_frame_from_string(frame_string);
}

void Frame::set_payload(unsigned char* new_payload) {
  memset(payload, 0, 1500);
  memcpy(payload, new_payload, 1500);
}

void Frame::set_payload(Packet packet) {
  packet.to_byte_string(payload);
}

/*
Setter for source address.
*/
void Frame::set_source(long int source) {
  source_address = source;
}

/*
Setter for destination address.
*/
void Frame::set_destination(long int destination) {
  destination_address = destination;
}

/*
Getter for source address.
*/
long int Frame::get_source_address() {
  return destination_address;  
}

/*
Getter for source address.
*/
long int Frame::get_destination_address() {
  return destination_address;
}

void Frame::get_payload(unsigned char* buffer) {
  memset(buffer, 0, 1500);
  memcpy(buffer, payload, 1500);
}

void Frame::get_byte_string(unsigned char* buffer) {
  memcpy(buffer, preamble, 7);
  memcpy(buffer+7, &SFD, 1);
  memcpy(buffer+8, &source_address, 6);
  memcpy(buffer+14, &destination_address, 6);
  memcpy(buffer+20, &length, 2);
  memcpy(buffer+22, payload, 1500);
  memcpy(buffer+1500+22, &CRC, 4);
}

void Frame::get_bit_string(unsigned char* buffer) {
  bytes_to_bits(buffer, preamble, 7);
  buffer += 7*9;
  bytes_to_bits(buffer, &SFD, 1);
  buffer += 1*9;
  bytes_to_bits(buffer, source_address, 6);
  buffer += 6*9;
  bytes_to_bits(buffer, destination_address, 6);
  buffer += 6*9;
  bytes_to_bits(buffer, length, 2);
  buffer += 2*9;
  bytes_to_bits(buffer, payload, 1500);
  buffer += 1500*9;
  bytes_to_bits(buffer, CRC, 4);
}

void Frame::instantiate_from_bit_string(unsigned char* buffer) {
  bits_to_bytes(preamble, buffer, 7);
  buffer += 7*9;
  bits_to_bytes(&SFD, buffer, 1);
  buffer += 1*9;
  bits_to_bytes(&source_address, buffer, 6);
  buffer += 6*9;
  bits_to_bytes(&destination_address, buffer, 6);
  buffer += 6*9;
  bits_to_bytes(&length, buffer);
  buffer += 2*9;
  bits_to_bytes(payload, buffer, 1500);
  buffer += 1500*9;
  bits_to_bytes(&CRC, buffer, 4);
}

void Frame::load_frame_from_string(unsigned char* frame_string) {
  memcpy(preamble, frame_string, 7);
  frame_string += 7;
  memcpy(&SFD, frame_string, 1);
  frame_string += 1;
  memcpy(&source_address, frame_string, 6);
  frame_string += 6;
  memcpy(&destination_address, frame_string, 6);
  frame_string += 6;
  memcpy(&length, frame_string, 2);
  frame_string += 2;
  memcpy(payload, frame_string, 1500);
  frame_string += 1500;
  memcpy(&CRC, frame_string, 4);
}

void Frame::load_packet(Packet* packet) {
  packet->load_packet_from_byte_string(payload);
}

/*
This method is used when generating a response
to a request. The source and destination must
simply be swapped.
*/
void Frame::swap_source_and_dest() {
  long int temp;
  temp = source_address;
  source_address = destination_address;
  destination_address = temp;
}

/*
This method is used to convert the 48-bit mac address
into a string that is user friendly.
Parameters:
  source - a boolean that determines whether or not to 
    return the source or destination address.
*/
unsigned char* Frame::address_to_string(bool source) {
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
