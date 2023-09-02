#ifndef _LOGGING_H_
#define _LOGGING_H_
#include <iostream>
#include <string>

void log(std::string message);

class Logger {
  std::string hostName;
  Logger(std::string hostName) {
    this->hostName = hostName;
  }
  void log(const char* message);
};

#endif