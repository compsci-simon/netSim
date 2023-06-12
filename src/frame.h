#ifndef _FRAME_H_
#define _FRAME_H_
#include <string>
#include "packet.h"

const int FRAME_PAYLOAD_SIZE = 1500;
const int FRAME_SIZE = FRAME_PAYLOAD_SIZE + 26;
const unsigned char ETHERNET_BROADCAST_ADDRESS[6] {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

class Frame {
  unsigned char preamble[7] {0};
  unsigned char SFD = 0b10101011;
  unsigned char source[6] {0};
  unsigned char destination[6] {0};
  short int length;
  unsigned char payload[FRAME_PAYLOAD_SIZE] {0};
  unsigned char CRC[4] {0};
public:
  Frame() {};
  Frame(unsigned char* string);
  void set_payload(unsigned char* new_payload);
  void set_payload(Packet packet);
  void get_payload(unsigned char* buffer);
  void set_source(unsigned char* source);
  void set_destination(unsigned char* destination);
  void get_byte_string(unsigned char* buffer);
  void load_frame_from_string(unsigned char* frame_string);
  void load_packet(Packet* packet);
};
#endif