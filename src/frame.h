#ifndef _FRAME_H_
#define _FRAME_H_
#include <string>

const int PAYLOAD_SIZE = 1500;
class Frame {
  unsigned char preamble[7] {0};
  unsigned char SFD = 0b10101011;
  unsigned char source[6] {0};
  unsigned char destination[6] {0};
  short int length;
  unsigned char payload[PAYLOAD_SIZE] {0};
  unsigned char CRC[4] {0};
public:
  Frame() {};
  Frame(char* string);
  void set_payload(unsigned char* new_payload);
  void set_source(unsigned char* source);
  void set_destination(unsigned char* destination);
  void to_string(char* sbuff);
  void load_frame_from_string(char* frame_string);
};
#endif