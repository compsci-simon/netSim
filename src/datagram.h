/*
The goal of the transport layer is to extend the host-to-host
message dilvery service of the underlying network to a process-
to-process message delivery service.

For more information see RFC 768 (https://www.rfc-editor.org/rfc/rfc768).
Format
------
                              Pseudo Header
                  0      7 8     15 16    23 24    31
                 +--------+--------+--------+--------+
                 |          source address           |
                 +--------+--------+--------+--------+
                 |        destination address        |
                 +--------+--------+--------+--------+
                 |  zero  |protocol|   UDP length    |
                 +--------+--------+--------+--------+
                 
                                Header
                  0      7 8     15 16    23 24    31
                 +--------+--------+--------+--------+
                 |     Source      |   Destination   |
                 |      Port       |      Port       |
                 +--------+--------+--------+--------+
                 |                 |                 |
                 |     Length      |    Checksum     |
                 +--------+--------+--------+--------+
                 |
                 |          data octets ...
                 +---------------- ...

                      User Datagram Header Format
*/ 

#ifndef _DATAGRAM_H_
#define _DATAGRAM_H_

#include "dhcp.h"

const int DATAGRAM_PAYLOAD_LENGTH = 1468;
const int DATAGRAM_LENGTH = 1476;

class Datagram {
  short int source_port = 0;
  short int destination_port = 0;
  short int length = 0;
  short int checksum = 0;
  unsigned char data[DATAGRAM_PAYLOAD_LENGTH];
public:
  void set_payload(unsigned char* buffer);
  void set_payload(DHCP_Message dhcp_message);
  void get_payload(unsigned char* buffer);
  void unencapsulate_dhcp_message(DHCP_Message* dhcp_message);
  void set_source_port(short int source_port);
  void set_destination_port(short int destination_port);
  short int get_source_port();
  short int get_destination_port();
  void get_bytestring(unsigned char* buffer);
  void instantiate_from_bytestring(unsigned char* byte_string);
};
#endif