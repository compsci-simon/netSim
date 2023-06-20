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
  int temp_addr = 0;
  int temp_pref = 0;
  for (int i = 0; i < num_addrs; i++) {
    temp_addr = rapl_vector.at(i*2);
    temp_pref = rapl_vector.at(i*2 + 1);
    memcpy(buffer, &temp_addr, 4);
    buffer += 4;
    memcpy(buffer, &temp_pref, 4);
    buffer += 4;
  }
}

void ICMP::add_addr_and_pref(int addr, int pref) {
  rapl_vector.push_back(addr);
  rapl_vector.push_back(pref);
}

void ICMP::instantiate_from_byte_string(unsigned char* buffer) {
  memcpy(&type, buffer, 1);
  buffer += 1;
  memcpy(&code, buffer, 1);
  buffer += 1;
  memcpy(&checksum, buffer, 2);
  buffer += 2;
  memcpy(&num_addrs, buffer, 1);
  buffer += 1;
  memcpy(&addr_entry_size, buffer, 1);
  buffer += 1;
  int temp_addr = 0;
  int temp_pref = 0;
  for (int i = 0; i < num_addrs; i++) {
    memcpy(&temp_addr, buffer, 4);
    buffer += 4;
    memcpy(&temp_pref, buffer, 4);
    buffer += 4;
    rapl_vector.push_back(temp_addr);
    rapl_vector.push_back(temp_pref);
  }
}

int ICMP::get_addr(int index) {
  return rapl_vector.at(index * 2);
}