#ifndef _UTILS_H_
#define _UTILS_H_
#include <iostream>
void generate_mac_address(unsigned char* buffer);
void bytes_to_bits(unsigned char* dest, unsigned char* source, ssize_t length);
void bytes_to_bits(unsigned char* dest, long* source, ssize_t length);
#endif