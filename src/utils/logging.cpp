#include "logging.h"
#include <fstream>
#include <chrono>

void log(std::string msg) {
  std::ofstream outputFile("/Users/simon/Desktop/netSim.log", std::ios::app);
  if (outputFile.is_open()) {
    outputFile << msg << std::endl;
    outputFile.close();
  } else {
    std::cerr << "Failed to open file" << std::endl;
  }
}

void log(const char* msg) {
  std::ofstream outputFile("/Users/simon/Desktop/netSim.log", std::ios::app);
  if (outputFile.is_open()) {
    outputFile << msg << std::endl;
    outputFile.close();
  } else {
    std::cerr << "Failed to open file" << std::endl;
  }
}

void Logger::log(const char* message) {
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
  char timeStr[100]; // Adjust the buffer size as needed
  std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));

  std::cout << "[" << timeStr << "]:" << hostName << ": " << message << std::endl;
}
