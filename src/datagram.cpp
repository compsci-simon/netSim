#include "datagram.h"
#include <cstring>

/*
This method sets the source port which identifies the application
on the sending host that will accept any returned datagram.
Parameters:
  source_port - The port on the source that will accept the returned datagram
*/
void Datagram::set_source_port(short int source_port) {
  this->source_port = source_port;
}

/*
This method sets the destination port which identifies the application
on the remote host that will accept the datagram.
Parameters:
  destination_port - The port on the destination that will accept the datagram
*/
void Datagram::set_destination_port(short int destination_port) {
  this->destination_port = destination_port;
}

/*
This method sets the payload of a datagram packet.
Parameters:
  buffer - A buffer of length exactly DATAGRAM_PAYLOAD_SIZE
*/
void Datagram::set_payload(unsigned char* buffer) {
  memset(data, 0, DATAGRAM_PAYLOAD_LENGTH);
  memcpy(data, buffer, DATAGRAM_PAYLOAD_LENGTH);
}

/*
This method gets the payload of a datagram packet.
Parameters:
  buffer - A buffer of length exactly DATAGRAM_PAYLOAD_SIZE
*/
void Datagram::get_payload(unsigned char* buffer) {
  memset(buffer, 0, DATAGRAM_PAYLOAD_LENGTH);
  memcpy(buffer, data, DATAGRAM_PAYLOAD_LENGTH);
}

/*
This method fills a provided buffer with the bytestring
representation of this datagram.
Parameters:
  buffer - The buffer that will be filled with the bytestring. This buffer should be exactly DATAGRAM_LENGTH in length
*/
void Datagram::get_bytestring(unsigned char* buffer) {
  memset(buffer, 0, DATAGRAM_LENGTH);
  memcpy(buffer, &source_port, 2);
  buffer += 2;
  memcpy(buffer, &destination_port, 2);
  buffer += 2;
  memcpy(buffer, &length, 2);
  buffer += 2;
  memcpy(buffer, &checksum, 2);
  buffer += 2;
  memcpy(buffer, data, DATAGRAM_PAYLOAD_LENGTH);
}

/*
This method is used to instantiate a datagram from a byte string.
Parameters:
  byte_string - The byte string that contains all the information necessary to instantiate the datagram.
*/
void Datagram::instantiate_from_bytestring(unsigned char* byte_string) {
  memcpy(&source_port, byte_string, 2);
  byte_string += 2;
  memcpy(&destination_port, byte_string, 2);
  byte_string += 2;
  memcpy(&length, byte_string, 2);
  byte_string += 2;
  memcpy(&checksum, byte_string, 2);
  byte_string += 2;
  memcpy(data, byte_string, DATAGRAM_PAYLOAD_LENGTH);
}
