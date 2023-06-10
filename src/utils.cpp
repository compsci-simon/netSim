#include "utils.h"
#include <iomanip>
#include <random>
#include <string>
#include <sstream>
#include <iostream>

#define MAC_ADDRESS_LENGTH 17

std::string generateHexChar() {
  std::random_device seed;
  std::mt19937 gen(seed());
  std::uniform_int_distribution<> dis(0, 15);
  std::ostringstream oss;
  oss << std::hex << dis(gen);
  return oss.str();
}

std::string generate_mac_address() {
  std::ostringstream oss;
  for (int i = 0; i < 6; i++) {
    oss << generateHexChar() << generateHexChar() << ":";
  }
  std::string macAddress = oss.str();
  macAddress.pop_back();
  return macAddress;
}