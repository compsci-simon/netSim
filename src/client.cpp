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
  char buffer[BUFFER_SIZE] = {0};
  struct sockaddr_in serverAddress;
  const char *greet = "Hi, from the client!";

  // Create socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

  socklen_t serverAddressLen = sizeof(serverAddress);

  // Connect to the server
  if (connect(sockfd, (struct sockaddr *)&serverAddress, serverAddressLen) < 0)
  {
    std::cerr << "Connection failed" << std::endl;
    return 1;
  }

  send(sockfd, greet, strlen(greet), 0);
  std::cout << "Message sent to server" << std::endl;
  read(sockfd, buffer, BUFFER_SIZE);
  std::cout << "Message received from server: " << buffer << std::endl;

  return 0;
}