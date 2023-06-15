#ifndef _FRAME_H_
#define _FRAME_H_
#include <string>
#include "packet.h"

class Frame {
  unsigned char preamble[7] {0};
  unsigned char SFD = 0b10101011;
  long int source_address = 0;
  long int destination_address = 0;
  short int length;
  unsigned char payload[1500] {0};
  unsigned char CRC[4] {0};
public:
  Frame() {};
  Frame(unsigned char* string);
  void set_payload(unsigned char* new_payload);
  void set_payload(Packet packet);
  void get_payload(unsigned char* buffer);
  void set_source(long int source);
  void set_destination(long int destination);
  long int get_source_address();
  long int get_destination_address();
  void get_byte_string(unsigned char* buffer);
  void load_frame_from_string(unsigned char* frame_string);
  void load_packet(Packet* packet);
  void swap_source_and_dest();
};
#endif