#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

const int PORT = 12345;
const int BUFFER_SIZE = 1024;

int main()
{
  int sockfd, clientfd, valRead;
  char buffer[BUFFER_SIZE] = {0};
  struct sockaddr_in serverAddress, clientAddress;
  int addrLen = sizeof(serverAddress);
  const char *greeting = "Hello from the server!";

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    std::cerr << "Error creating socket" << std::endl;
    return 1;
  }

  // Bind socket to IP and port
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = INADDR_ANY;
  serverAddress.sin_port = htons(PORT);
  if (bind(sockfd, (struct sockaddr *)&serverAddress, addrLen) < 0)
  {
    std::cerr << "Error binding socket" << std::endl;
    return 1;
  }

  if (listen(sockfd, 3) < 0)
  {
    std::cerr << "Listen failed" << std::endl;
    return -1;
  }

  socklen_t client_len;

  if ((clientfd = accept(sockfd, (struct sockaddr *)&serverAddress, &client_len)) < 0)
  {
    std::cerr << "Accept failed" << std::endl;
    return 1;
  }

  // Receive and print messages
  valRead = read(clientfd, buffer, BUFFER_SIZE);
  std::cout << "Received: " << buffer << std::endl;
  send(clientfd, greeting, strlen(greeting), 0);
  std::cout << "Response sent" << std::endl;

  close(sockfd);

  return 0;
}