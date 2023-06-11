#include "utils.h"
#include <iomanip>
#include <random>
#include <string>
#include <sstream>
#include <iostream>

void generate_mac_address(unsigned char* buffer) {
  memset(buffer, 0, 6);
  std::random_device seed;
  std::mt19937 gen(seed());
  std::uniform_int_distribution<> dis(0, 1);
  for (int byte = 0; byte < 6; byte++) {
    for (int bit = 0; bit < 8; bit++) {
      buffer[byte] = buffer[byte] << 1;
      buffer[byte] = buffer[byte] | dis(gen);
    }
  }
}