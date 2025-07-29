#pragma once

#include "./packet.h"

namespace itp_packet {
class IdentifyCDRequestPacket : public Packet {
 public:
  static IdentifyCDRequestPacket &instance() {
    static IdentifyCDRequestPacket instance;
    return instance;
  }
  using Packet::Packet;

 private:
  IdentifyCDRequestPacket() : Packet(RawPacket(PacketType::IDENTIFY_REQUEST, 1)) { pkt_.set_payload_byte(0, 0xCD); }
};

class IdentifyCDResponsePacket : public Packet {
  using Packet::Packet;

 public:
  std::string to_string() const override;
};
}  // namespace itp_packet