#include <iostream>
#include "icmp.h"

void ICMP::get_byte_string(unsigned char* buffer) {
  memset(buffer, 0, 10);
  memcpy(buffer, &type, 1);
  buffer += 1;
  memcpy(buffer, &code, 1);
  buffer += 1;
  memcpy(buffer, &checksum, 2);
  buffer += 2;
  memcpy(buffer, &num_addrs, 1);
  buffer += 1;
  memcpy(buffer, &addr_entry_size, 1);
  buffer += 1;
}