#pragma once

#include "itp_packet.h"

namespace itp_packet {
class ConnectRequestPacket : public Packet {
 public:
  using Packet::Packet;
  static ConnectRequestPacket &instance() {
    static ConnectRequestPacket instance;
    return instance;
  }

  std::string to_string() const override;

 private:
  ConnectRequestPacket() : Packet(RawPacket(PacketType::CONNECT_REQUEST, 2)) {
    pkt_.set_payload_byte(0, 0xca);
    pkt_.set_payload_byte(1, 0x01);
  }
};

class ConnectResponsePacket : public Packet {
 public:
  using Packet::Packet;
  std::string to_string() const override;
};

}  // namespace itp_packet