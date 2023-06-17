#ifndef _UTILS_H_
#define _UTILS_H_
#include <sys/types.h>

void bytes_to_bits(unsigned char* dest, unsigned char* source, ssize_t length);
void bytes_to_bits(unsigned char* dest, long source, ssize_t length);
void bytes_to_bits(unsigned char* dest, int source);
void bytes_to_bits(unsigned char* dest, short int source);
void bytes_to_bits(unsigned char* dest, unsigned char source);
void bits_to_bytes(unsigned char* dest, unsigned char* source, ssize_t length);
void bits_to_bytes(long* dest, unsigned char* source, ssize_t length);
void bits_to_bytes(int *dest, unsigned char* source, ssize_t length);
void bits_to_bytes(short int *dest, unsigned char* source);
void bits_to_bytes(unsigned char *dest, unsigned char* source);
#endif