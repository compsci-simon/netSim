#include "dhcp.h"

DHCP_Message::DHCP_Message() {
  options[0] = 99;
  options[1] = 130;
  options[2] = 83;
  options[3] = 99;
}