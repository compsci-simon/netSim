#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

const int PORT = 12345;
const int BUFFER_SIZE = 1024;

int main()
{
  int sockfd;
  char buffer[BUFFER_SIZE];

  struct sockaddr_in serverAddress, clientAddress;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    std::cerr << "Error creating socket" << std::endl;
    return 1;
  }

  // Bind socket to IP and port
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(PORT);
  if (bind(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress) < 0))
  {
    std::cerr << "Error binding socket" << std::endl;
    return 1;
  }

  // Receive and print messages
  while (true)
  {
    unsigned int len = sizeof(clientAddress);
    ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &len);
    if (n < 0)
    {
      std::cerr << "Error receiving data" << std::endl;
      return 1;
    }
    std::cout << "Received message: " << buffer << std::endl;
  }

  close(sockfd);

  return 0;
}