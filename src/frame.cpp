#include <iostream>
#include<array>
#include "frame.h"

Frame::Frame(char* frame_string) {
  load_frame_from_string(frame_string);
}

void Frame::set_source(unsigned char* source) {
  memcpy(Frame::source, source, 6);
}

void Frame::set_destination(unsigned char* destination) {
  memcpy(Frame::destination, destination, 6);
}

void Frame::set_payload(unsigned char* new_payload) {
  memset(payload, 0, PAYLOAD_SIZE);
  memcpy(payload, new_payload, PAYLOAD_SIZE);
}

void Frame::to_string(char* sbuff) {
  memcpy(sbuff, preamble, 7);
  memcpy(sbuff+7, &SFD, 1);
  memcpy(sbuff+8, source, 6);
  memcpy(sbuff+14, destination, 6);
  memcpy(sbuff+20, &length, 2);
  memcpy(sbuff+22, payload, PAYLOAD_SIZE);
  memcpy(sbuff+PAYLOAD_SIZE+22, CRC, 4);
}

void Frame::load_frame_from_string(char* frame_string) {
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
  memcpy(payload, frame_string, PAYLOAD_SIZE);
  frame_string += PAYLOAD_SIZE;
  memcpy(CRC, frame_string, 4);
}
