#include "router.h"
#include "../protocols/dhcp.h"
#include "../protocols/arp.h"
#include "../protocols/ethernet.h"
#include "../protocols/ip.h"
#include "../protocols/icmp.h"
#include "../protocols/datagram.h"

#include "switch.h"
// const unsigned char IP[4] = { 192, 168, 0, 1 };

Router::Router() {
  macAddress = 0x0001234455667;
  this->dhcp_server = new DHCP_Server();
  this->dhcp_server->set_router(this);
  this->network_switch = new Switch(this);
  this->network_switch->switch_on();
}

void Router::send_frame_to_switch(Ethernet* frame) {
  network_switch->send_frame(frame);
}

void Router::interrupt(Interrupt interrupt) {
  switch (interrupt) {
    case FRAME_RECEIVED:
      std::cout << "Frame interrupt received" << std::endl;
      Ethernet* frame = network_switch->get_frame();
      process_frame(frame);
      free(frame);
      break;
  }
}

void Router::process_frame(Ethernet* frame) {
  if (frame->get_destination_address() == 0xffffffffffff || frame->get_destination_address() == macAddress) {
    if (frame->get_type() == 0x0800) {
      IP packet;
      // Process IP Packet
      frame->decapsulate(&packet);
      process_packet(frame, packet);
    } else if (frame->get_type() == 0x0806) {
      // Process ARP Query
      Arp query;
      frame->decapsulate(&query);
      process_query(frame, query);
    } else {
      std::cerr << "Received frame with unknow protocol " << frame->get_type_string() << std::endl;
    }
  } else {
    std::cerr << "Received a frame with a destination not equal to router MAC or broadcast MAC. Ethernet destination address = " << frame->address_to_string(false) << std::endl;
  }
}

void Router::process_packet(Ethernet* frame, IP packet) {
  if (packet.get_destination() == 0xffffffff || packet.get_destination() == ip_addr 
      || packet.get_destination() == 0xe0000001) {
    if (packet.get_protocol() == 1) {
      ICMP message;
      packet.unencapsulate(&message);
      process_message(frame, message);
    } else if (packet.get_protocol() == 17) {
      Datagram datagram;
      packet.load_datagram(&datagram);
      process_datagram(frame, datagram);
    } else {
      std::cerr << "Received packet with unknown destination protocol " << packet.get_protocol() << std::endl;
    }
  } else {
    char buffer[17] {0};
    IP::address_to_string(packet.get_destination(), buffer);
    std::cerr << "Received a packet with a destination not equal to router IP or broadcast. Packet destination IP = " << buffer << std::endl;
    // Silently dismiss packet
  }
}

void Router::process_query(Ethernet* frame, Arp query) {
  char buf[17] {0};
  IP::address_to_string(query.get_target_protocol(), buf);
  std::cout << "Received ARP query targetted for " << buf << std::endl;
  if (query.get_target_protocol() == ip_addr) {
    Ethernet frame;
    query.set_target_hardware(macAddress);

    frame.encapsulate(query);
    frame.set_destination_address(query.get_source_hardware());
    frame.set_source_address(macAddress);
    frame.set_type(0x0806);
    send_frame(&frame);
    std::cout << "Sending ARP reply" << std::endl;
  } else {
    std::cout << "Dismissing ARP query." << std::endl;
  }
}

void Router::process_message(Ethernet* source_frame, ICMP message) {
  ICMP new_message;
  IP new_packet, old_packet;
  Ethernet* new_frame = new Ethernet();
  if (message.get_type() == 0) {
    // PING Reply
    source_frame->decapsulate(&old_packet);
    std::cout << "Received ping reply from " << old_packet.address_to_string(true) << std::endl;
  } if (message.get_type() == 8) {
    // PING Request
    source_frame->decapsulate(&old_packet);

    std::cout << "Received PING from " << old_packet.address_to_string(true) << std::endl;

    new_message.set_type(0);
    new_message.set_code(0);

    new_packet.encapsulate(new_message);
    new_packet.set_destination(old_packet.get_source());
    new_packet.set_source(ip_addr);
    new_packet.set_protocol(1);

    new_frame->encapsulate(new_packet);
    new_frame->set_source_address(macAddress);
    new_frame->set_destination_address(source_frame->get_source_address());
    new_frame->set_type(0x0800);
    send_frame(new_frame);

  } else if (message.get_type() == 10) {
    // Router solicitation received
    if (message.get_code() != 0) {
      std::cerr << "Received ICMP message with invalid code " << message.get_code() << std::endl;
      return;
    }
    // TODO: Do additional check to make sure the length of the ICMP message is no longer than 8 using the packet

    new_message.set_type(9);
    new_message.set_num_addrs(1);
    new_message.set_addr_entry_size(2);
    new_message.add_addr_and_pref(ip_addr, 1);

    new_packet.set_source(ip_addr);
    new_packet.set_destination(0xe0000001);
    new_packet.set_protocol(1);
    new_packet.encapsulate(new_message);

    new_frame->encapsulate(new_packet);
    new_frame->set_source_address(macAddress);
    new_frame->set_destination_address(source_frame->get_source_address());
    new_frame->set_type(0x0800);
    send_frame(new_frame);

  } else {
    std::cerr << "Received ICMP message with unknown type " << message.get_type() << std::endl;
    return;
  }
}

void Router::process_datagram(Ethernet* frame, Datagram datagram) {
  if (datagram.get_destination_port() == 67) {
    DHCP_Message message;
    datagram.unencapsulate_dhcp_message(&message);
    this->dhcp_server->handle_message(*frame, message);
  } else {
    std::cerr << "datagram received with unknown destination port" << datagram.get_destination_port() << std::endl;
    return;
  }
}

/*
This method is used to send the router's
frame.
*/
void Router::send_frame(Ethernet* frame) {
  network_switch->send_frame(frame);
}
