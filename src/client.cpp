#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

const int PORT = 12345;
const int BUFFER_SIZE = 1024;

int main()
{
  int sockfd;
  char buffer[BUFFER_SIZE];
  struct sockaddr_in serverAddress;

  // Create socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    std::cerr << "Error creating socket" << std::endl;
    return 1;
  }

  // Bind socket to IP and PORT
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(PORT);
  if (inet_pton(AF_INET, "localhost", &(serverAddress.sin_addr)) < 0)
  {
    std::cerr << "Invalid address" << std::endl;
    return 1;
  }

  // Send messages to server
  while (true)
  {
    std::cout << "Enter a message: ";
    std::cin.getline(buffer, BUFFER_SIZE);

    ssize_t n = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (n < 0)
    {
      std::cerr << "Error sending data" << std::endl;
      return 1;
    }
  }

  close(sockfd);

  return 0;
}