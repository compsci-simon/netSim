#include "utils.h"
#include <iostream>

void bytes_to_bits(unsigned char* dest, unsigned char* source, ssize_t length) {
  memset(dest, 0, length * 9);
  for (int i = 0; i < length; i++) {
    for (int j = 0; j < 8; j++) {
      unsigned char a = (unsigned char) ((source[i] >> (7 - j)) & 1);
      unsigned char b = a + 48;
      dest[i*9 + j] = b;
    }
    dest[i*9 + 8] = ' ';
  }
}

void bytes_to_bits(unsigned char* dest, long source, ssize_t length) {
  for (int i = 0; i < length; i++) {
    for (int j = 0; j < 8; j++) {
      unsigned char val = (unsigned char) (((source >> (8*(length - i - 1) + 7 - j)) & 1) + 48);
      dest[i*9 + j] = val;
    }
    dest[i*9 + 8] = ' ';
  }
}

void bytes_to_bits(unsigned char* dest, int source) {
  ssize_t length = 4;
  for (int i = 0; i < length; i++) {
    for (int j = 0; j < 8; j++) {
      unsigned char val = (unsigned char) (((source >> (8*(length - i - 1) + 7 - j)) & 1) + 48);
      dest[i*9 + j] = val;
    }
    dest[i*9 + 8] = ' ';
  }
}

void bytes_to_bits(unsigned char* dest, short int source) {
  ssize_t length = 2;
  for (int i = 0; i < length; i++) {
    for (int j = 0; j < 8; j++) {
      unsigned char val = (unsigned char) (((source >> (8*(length - i - 1) + 7 - j)) & 1) + 48);
      dest[i*9 + j] = val;
    }
    dest[i*9 + 8] = ' ';
  }
}

void bytes_to_bits(unsigned char* dest, unsigned char source) {
  for (int j = 0; j < 8; j++) {
    unsigned char val = (unsigned char) (((source >> (7 - j)) & 1) + 48);
    dest[j] = val;
  }
  dest[8] = ' ';
}

void bits_to_bytes(unsigned char* dest, unsigned char* source, ssize_t length) {
  memset(dest, 0, length);
  for (int byte = 0; byte < length; byte++) {
    for (int bit = 0; bit < 8; bit++) {
      dest[byte] = dest[byte] | ((source[byte*9 + bit] - 48) << (7 - bit));
    }
  }
}

void bits_to_bytes(long *dest, unsigned char* source, ssize_t length) {
  *dest = 0;
  for (int byte = 0; byte < length; byte++) {
    (*dest) = (*dest) << 8;
    for (int bit = 0; bit < 8; bit++) {
      *dest = *dest | ((source[byte*9 + bit] - 48) << (7 - bit));
    }
  }
}

void bits_to_bytes(int *dest, unsigned char* source, ssize_t length) {
  *dest = 0;
  for (int byte = 0; byte < length; byte++) {
    (*dest) = (*dest) << 8;
    for (int bit = 0; bit < 8; bit++) {
      *dest = *dest | ((source[byte*9 + bit] - 48) << (7 - bit));
    }
  }
}

void bits_to_bytes(short int *dest, unsigned char* source) {
  *dest = 0;
  for (int byte = 0; byte < 2; byte++) {
    (*dest) = (*dest) << 8;
    for (int bit = 0; bit < 8; bit++) {
      *dest = *dest | ((source[byte*9 + bit] - 48) << (7 - bit));
    }
  }
}

void bits_to_bytes(unsigned char *dest, unsigned char* source) {
  *dest = 0;
  for (int bit = 0; bit < 8; bit++) {
    *dest = *dest | ((source[bit] - 48) << (7 - bit));
  }
}
