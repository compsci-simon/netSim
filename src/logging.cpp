#include "logging.h"
#include <fstream>

void log(std::string msg) {
  std::ofstream outputFile("/Users/simon/Desktop/netSim.log", std::ios::app);
  if (outputFile.is_open()) {
    outputFile << msg << std::endl;
    std::cout << "Done logging" << std::endl;
    outputFile.close();
  } else {
    std::cerr << "Failed to open file" << std::endl;
  }
}