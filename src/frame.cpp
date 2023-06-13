#include <iostream>
#include "frame.h"

Frame::Frame(unsigned char* frame_string) {
  load_frame_from_string(frame_string);
}

void Frame::set_source(unsigned char* source) {
  memcpy(Frame::source, source, 6);
}

void Frame::set_destination(unsigned char* destination) {
  memcpy(Frame::destination, destination, 6);
}

void Frame::set_payload(unsigned char* new_payload) {
  memset(payload, 0, FRAME_PAYLOAD_SIZE);
  memcpy(payload, new_payload, FRAME_PAYLOAD_SIZE);
}

void Frame::set_payload(Packet packet) {
  packet.to_byte_string(payload);
}

void Frame::get_payload(unsigned char* buffer) {
  memset(buffer, 0, FRAME_PAYLOAD_SIZE);
  memcpy(buffer, payload, FRAME_PAYLOAD_SIZE);
}

void Frame::get_byte_string(unsigned char* buffer) {
  memcpy(buffer, preamble, 7);
  memcpy(buffer+7, &SFD, 1);
  memcpy(buffer+8, source, 6);
  memcpy(buffer+14, destination, 6);
  memcpy(buffer+20, &length, 2);
  memcpy(buffer+22, payload, FRAME_PAYLOAD_SIZE);
  memcpy(buffer+FRAME_PAYLOAD_SIZE+22, CRC, 4);
}

void Frame::load_frame_from_string(unsigned char* frame_string) {
  memcpy(preamble, frame_string, 7);
  frame_string += 7;
  memcpy(&SFD, frame_string, 1);
  frame_string += 1;
  memcpy(source, frame_string, 6);
  frame_string += 6;
  memcpy(destination, frame_string, 6);
  frame_string += 6;
  memcpy(&length, frame_string, 2);
  frame_string += 2;
  memcpy(payload, frame_string, FRAME_PAYLOAD_SIZE);
  frame_string += FRAME_PAYLOAD_SIZE;
  memcpy(CRC, frame_string, 4);
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
  unsigned char temp[6] {0};
  memcpy(temp, source, 6);
  memcpy(source, destination, 6);
  memcpy(destination, temp, 6);
}
