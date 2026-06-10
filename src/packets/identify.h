#pragma once

#include "itp_packet.h"

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
  static bool validate_type(const RawPacket &pkt) {
    return pkt.get_packet_type() == static_cast<uint8_t>(PacketType::IDENTIFY_RESPONSE);
  }
};
}  // namespace itp_packet