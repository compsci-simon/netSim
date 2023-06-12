/*
This file implements the dynamic host protocol configuration described 
in detail in RFC 2131 (https://www.rfc-editor.org/rfc/rfc2131).

   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     op (1)    |   htype (1)   |   hlen (1)    |   hops (1)    |
   +---------------+---------------+---------------+---------------+
   |                            xid (4)                            |
   +-------------------------------+-------------------------------+
   |           secs (2)            |           flags (2)           |
   +-------------------------------+-------------------------------+
   |                          ciaddr  (4)                          |
   +---------------------------------------------------------------+
   |                          yiaddr  (4)                          |
   +---------------------------------------------------------------+
   |                          siaddr  (4)                          |
   +---------------------------------------------------------------+
   |                          giaddr  (4)                          |
   +---------------------------------------------------------------+
   |                                                               |
   |                          chaddr  (16)                         |
   |                                                               |
   |                                                               |
   +---------------------------------------------------------------+
   |                                                               |
   |                          sname   (64)                         |
   +---------------------------------------------------------------+
   |                                                               |
   |                          file    (128)                        |
   +---------------------------------------------------------------+
   |                                                               |
   |                          options (variable)                   |
   +---------------------------------------------------------------+

*/
#ifndef _DHCP_H_
#define _DHCP_H_

const unsigned char BOOT_REQUEST = 1;
const unsigned char BOOT_REPLY = 2;
const int DHCP_LENGTH = 1468;
const int DHCP_OPTIONS_LENGTH = 1232;


class DHCP_Server {
  unsigned char available_ips[255] {0};
};

class DHCP_Message {
  unsigned char op;
  unsigned char htype = 1;
  unsigned char hlen = 6;
  unsigned char hops = 0;
  int xid = 0;
  short int secs = 0;
  short int flags = 0;
  int ciaddr = 0;
  int yiaddr = 0;
  int siaddr = 0;
  int giaddr = 0;
  unsigned char chaddr[16] {0};
  unsigned char sname[64] {0};
  unsigned char file[128] {0};
  unsigned char options[DHCP_OPTIONS_LENGTH] {0};
public:
  DHCP_Message();
  void set_op(unsigned char operation);
};

#endif