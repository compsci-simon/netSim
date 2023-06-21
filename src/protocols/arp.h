#ifndef _ARP_H_
#define _ARP_H_
class Arp {
  short int hardware_type = 1;
  short int protocol_type = 0x0800;
  unsigned char hlen = 48;
  unsigned char plen = 32;
  short int operation;
  long sourceHardwareAddr {0}; // Sender MAC
  int sourceProtocolAddr {0}; // Sender IP
  long targetHardwareAddr {0};
  int targetProtocolAddr {0};
public:
  void set_source_hardware(long int addr) { sourceHardwareAddr = addr; }
  void set_source_protocol(int addr) { sourceProtocolAddr = addr; }
  void set_target_hardware(long int addr) { targetHardwareAddr = addr; }
  void set_target_protocol(int addr) { targetProtocolAddr = addr; }
  void set_operation(short int operation) { this->operation = operation; }
  long int get_source_hardware() { return sourceHardwareAddr; }
  int get_source_protocol() { return sourceProtocolAddr; }
  long int get_target_hardware() { return targetHardwareAddr; }
  int get_target_protocol() { return targetProtocolAddr; }
  void get_byte_string(unsigned char* buffer);
  void instantiate_from_byte_string(unsigned char* buffer);
};
#endif

/*
For more information see RFC 6474 (https://www.rfc-editor.org/rfc/rfc6747#section-1.1).

        0        7        15       23       31
        +--------+--------+--------+--------+
        |       HT        |        PT       |
        +--------+--------+--------+--------+
        |  HAL   |  PAL   |        OP       |
        +--------+--------+--------+--------+
        |         S_HA (bytes 0-3)          |
        +--------+--------+--------+--------+
        | S_HA (bytes 4-5)|S_L32 (bytes 0-1)|
        +--------+--------+--------+--------+
        |S_L32 (bytes 2-3)| T_HA (bytes 0-1)|
        +--------+--------+--------+--------+
        |         T_HA (bytes 3-5)          |
        +--------+--------+--------+--------+
        |         T_L32 (bytes 0-3)         |
        +--------+--------+--------+--------+

    Figure 2.1: ILNPv4 ARP Request packet format
*/