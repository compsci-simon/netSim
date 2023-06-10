#include <iostream>
#include<array>
#include "frame.h"

Frame::Frame(char* frame_byte_string) {
  int stringlen = PAYLOAD_SIZE + 26;
  if (strlen(frame_byte_string) != stringlen) {
    std::cerr << "The source string provided was not 1526 characters long. Found " << strlen(frame_byte_string) << " characters" << std::endl;
    return;
  }
  memcpy(string_buffer, frame_byte_string, stringlen);
  memcpy(preamble, frame_byte_string, 7);
  frame_byte_string += 7;
  memcpy(&SFD, frame_byte_string, 1);
  frame_byte_string += 1;
  memcpy(source, frame_byte_string, 6);
  frame_byte_string += 6;
  memcpy(destination, frame_byte_string, 6);
  frame_byte_string += 6;
  memcpy(&length, frame_byte_string, 2);
  frame_byte_string += 2;
  memcpy(payload, frame_byte_string, PAYLOAD_SIZE);
  frame_byte_string += PAYLOAD_SIZE;
  memcpy(CRC, frame_byte_string, 4);
}

void Frame::set_source(unsigned char* source) {
  memcpy(Frame::source, source, 6);
}

void Frame::set_destination(unsigned char* destination) {
  memcpy(Frame::destination, destination, 6);
}

void Frame::set_payload(unsigned char* new_payload) {
  memcpy(payload, new_payload, PAYLOAD_SIZE);
}

unsigned char* Frame::to_string() {
  unsigned char* frame_byte_string = new unsigned char[PAYLOAD_SIZE + 26] {0};
  memcpy(frame_byte_string, preamble, 7);
  memcpy(frame_byte_string+7, &SFD, 1);
  memcpy(frame_byte_string+8, source, 6);
  memcpy(frame_byte_string+14, destination, 6);
  memcpy(frame_byte_string+20, &length, 2);
  memcpy(frame_byte_string+22, payload, PAYLOAD_SIZE);
  memcpy(frame_byte_string+PAYLOAD_SIZE+22, CRC, 4);
  return frame_byte_string;
}

int main() {
  Frame f;
  unsigned char new_payload[] {1, 2, 3};
  f.set_payload(new_payload);
}