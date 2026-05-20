#pragma once

#include "itp_packet.h"

namespace itp_packet {

// Number of zone bytes in packet
static const uint8_t ZONE_BYTES_COUNT = 8;

class ZoneGetResponsePacket : public Packet {
  static const int PLINDEX_ZONE_OFFSET = 3;

  using Packet::Packet;

 public:
  bool get_zone_active(uint8_t zone) const { return pkt_.get_payload_byte(PLINDEX_ZONE_OFFSET + zone) != 0; }

  std::string to_string() const override;
};

class ZoneSetRequestPacket : public Packet {
  static const int PLINDEX_ZONE_MASK = 1;
  static const int PLINDEX_ZONE_OFFSET = 3;

 public:
  ZoneSetRequestPacket() : Packet(RawPacket(PacketType::SET_REQUEST, 16)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(SetCommand::ZONE_STATE));
  }
  using Packet::Packet;

  ZoneSetRequestPacket &set_zone_active(uint8_t zone, bool active);

  std::string to_string() const override;
};

}  // namespace itp_packet
