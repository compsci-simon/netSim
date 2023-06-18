#include "arp.h"
#include <iostream>

/*
This method is used when multiplexing an ARP query. After
the ARP query has ben resolved to a byte string, it can be multiplexed
by the the hardware layer.
Parameters:
  buffer - A buffer for the string. No less than 28 bytes.
*/
void Arp::get_byte_string(unsigned char* buffer) {
  memset(buffer, 0, 28);
  memcpy(buffer, &hardware_type, 2);
  buffer += 2;
  memcpy(buffer, &protocol_type, 2);
  buffer += 2;
  memcpy(buffer, &hlen, 1);
  buffer += 1;
  memcpy(buffer, &plen, 1);
  buffer += 1;
  memcpy(buffer, &operation, 2);
  buffer += 2;
  memcpy(buffer, &sourceHardwareAddr, 6);
  buffer += 6;
  memcpy(buffer, &sourceProtocolAddr, 4);
  buffer += 4;
  memcpy(buffer, &targetHardwareAddr, 6);
  buffer += 6;
  memcpy(buffer, &targetProtocolAddr, 4);
  buffer += 4;
}

/*
This method is used when demultiplexing an ARP query.
It takes as input a bytestring that contains all the information
needed for the query.
Parameters:
  buffer - The byte string.
*/
void Arp::instantiate_from_byte_string(unsigned char* buffer) {
  memcpy(&hardware_type, buffer, 2);
  buffer += 2;
  memcpy(&protocol_type, buffer, 2);
  buffer += 2;
  memcpy(&hlen, buffer, 1);
  buffer += 1;
  memcpy(&plen, buffer, 1);
  buffer += 1;
  memcpy(&operation, buffer, 2);
  buffer += 2;
  memcpy(&sourceHardwareAddr, buffer, 6);
  buffer += 6;
  memcpy(&sourceProtocolAddr, buffer, 4);
  buffer += 4;
  memcpy(&targetHardwareAddr, buffer, 6);
  buffer += 6;
  memcpy(&targetProtocolAddr, buffer, 4);
  buffer += 4;
}