#ifndef _ETHERNET_H_
#define _ETHERNET_H_
#include <string>

class Arp;
class IP;

class Ethernet {
  unsigned char preamble[7] { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa };
  unsigned char SFD = 0b10101011;
  long int source_address = 0;
  long int destination_address = 0;
  short int type;
  unsigned char payload[1500] {0};
  int CRC {0};
  unsigned char address_string[18] {0};
public:
  Ethernet() {};
  Ethernet(unsigned char* string);
  void encapsulate(IP packet);
  void decapsulate(IP* packet);
  void encapsulate(Arp query);
  void decapsulate(Arp* query);
  void load_frame_from_string(unsigned char* frame_string);
  void get_bit_string(unsigned char* buffer);
  void instantiate_from_bit_string(unsigned char* buffer);
  unsigned char* address_to_string(bool source);
  unsigned char* get_type_string();
  short int get_type() { return type; }
  long int get_source_address() { return source_address; }
  long int get_destination_address() { return destination_address; }
  void set_type(short int t) { type = t; }
  void set_source_address(long int source) { source_address = source; }
  void set_destination_address(long int destination) { destination_address = destination; }
  static std::string long_to_address(long address);
};
#endif