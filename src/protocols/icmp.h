
  //  ICMP Router Advertisement Message

  //      0                   1                   2                   3
  //      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //     |     Type      |     Code      |           Checksum            |
  //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //     |   Num Addrs   |Addr Entry Size|           Lifetime            |
  //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //     |                       Router Address[1]                       |
  //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //     |                      Preference Level[1]                      |
  //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //     |                       Router Address[2]                       |
  //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //     |                      Preference Level[2]                      |
  //     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //     |                               .                               |
  //     |                               .                               |
  //     |                               .                               |

#ifndef _ICMP_H_
#define _ICMP_H_

#include <vector>

class ICMP {
  unsigned char type {0};
  unsigned char code {0};
  short int checksum {0};
  unsigned char num_addrs {0};
  unsigned char addr_entry_size {0};
  short int lifetime {0};
  short int identifier {0};
  short int seq_num {0};
  // Router Address and Preference Level vector
  std::vector<int> rapl_vector;
public:
  void set_type(unsigned char type) { this->type = type; }
  void set_code(unsigned char code) { this->code = code; }
  void set_num_addrs(unsigned char num_addrs) { this-> num_addrs = num_addrs; }
  void set_addr_entry_size(unsigned char addr_entry_size) { this->addr_entry_size = addr_entry_size; }
  void set_lifetime(short int lifetime) { this->lifetime = lifetime; }
  void set_identifier(short int identifier) { this->identifier = identifier; }
  void set_seq_num(short int seq_num) { this->seq_num = seq_num; }
  unsigned char get_type() { return type; }
  unsigned char get_code() { return code; }
  unsigned char get_num_addrs() { return num_addrs; }
  unsigned char get_addr_entry_size() { return addr_entry_size; }
  short int get_lifetime() { return lifetime; }
  short int get_identifier() { return identifier; }
  short int get_seq_num() { return seq_num; }
  void get_byte_string(unsigned char* buffer);
  void instantiate_from_byte_string(unsigned char* buffer);
  void add_addr_and_pref(int addr, int pref);
  int get_addr(int index);
};

#endif