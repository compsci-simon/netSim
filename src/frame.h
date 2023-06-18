#ifndef _FRAME_H_
#define _FRAME_H_
#include <string>

class Arp;
class Packet;

class Frame {
  unsigned char preamble[7] { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };
  unsigned char SFD = 0b10101011;
  long int source_address = 0;
  long int destination_address = 0;
  short int type;
  unsigned char payload[1500] {0};
  int CRC {0};
  unsigned char address_string[18] {0};
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
  void get_bit_string(unsigned char* buffer);
  void instantiate_from_bit_string(unsigned char* buffer);
  void load_packet(Packet* packet);
  void swap_source_and_dest();
  unsigned char* address_to_string(bool source);
  short int get_type() { return type; }
  unsigned char* get_type_string();
  void set_type(short int t) { type = t; }
  void multiplex(Arp query);
  void demultiplex(Arp* query);
};
#endif