#ifndef _IP_H_
#define _IP_H_

class Datagram;
class ICMP;

const int IP_PAYLOAD_SIZE = 1472;
const int IP_SIZE = 1500;
const int IP_BROADCAST = 0b11111111'11111111'11111111'11111111;

class IP {
  unsigned char V_IHL = 0b01000101;
  unsigned char TOS = 0b00000000;
  short int total_length = 0;
  short int identification = 0;
  short int flags_and_offset = 0;
  unsigned char TTL = 123;
  unsigned char protocol = 0;
  short int header_checksum = 0;
  int source_address = 0;
  int destination_address = 0;
  long int options = 0;
  unsigned char data[IP_PAYLOAD_SIZE];
  char address_string[17];
public:
  IP();
  void set_destination(const char* address);
  void set_destination(int address);
  void set_source(int address);
  void set_payload(unsigned char* buffer);
  void set_payload(Datagram datagram);
  void set_options(long int options) { this->options = options; }
  void set_protocol(int proto) { protocol = proto; }
  int get_protocol() { return protocol; }
  void get_payload(unsigned char* buffer);
  int get_source() { return source_address; }
  int get_destination() { return destination_address; }
  void to_byte_string(unsigned char* buffer);
  void load_packet_from_byte_string(unsigned char* byte_string);
  void load_datagram(Datagram* datagram);
  char* address_to_string(bool source);
  static void address_to_string(int address, char* buffer);
  void encapsulate(ICMP message);
};
#endif

// IP packet specification. see RFC 791 (https://www.rfc-editor.org/rfc/rfc791)
// for more information.
// Version (4 bits) - The version indicates the format of the internet header. We use version 4
// Internet Header Length (IHL) (4 bits) - The minimum value for a correct header is 5
// Type of Server (8 bits) - The Type of Service provides an indication of the abstract
// parameters of the quality of service desired.
// Total length (16 bits) - An identifying value assigned by the sender to aid in assembling the
// fragments of a datagram.
// Identification (16 bits) - An identifying value assigned by the sender to aid in assembling the
// fragments of a datagram.
// Flags (3 bits) - Various Control Flags.
// Fragment offset (13 bits)
// 
// 
// 0                   1                   2                   3
//     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |Version|  IHL  |Type of Service|          Total Length         |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |         Identification        |Flags|      Fragment Offset    |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |  Time to Live |    Protocol   |         Header Checksum       |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                       Source Address                          |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                    Destination Address                        |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//    |                             Options                           |
//    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// For now we will avoid packet fragmentation and set the payload size to
// a fixed 1476 bytes. 24