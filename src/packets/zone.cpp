#include "zone.h"

namespace itp_packet {

ZoneSetRequestPacket &ZoneSetRequestPacket::set_zone_active(uint8_t zone, bool active) {
  pkt_.set_payload_byte(PLINDEX_ZONE_MASK, 1 << zone);
  pkt_.set_payload_byte(PLINDEX_ZONE_OFFSET + zone, active ? 1 : 0);
  return *this;
}

std::string ZoneSetRequestPacket::to_string() const {
  std::string result = "Zone Set Request: " + Packet::to_string() + "\n " + CONSOLE_COLOR_PURPLE + "Zones:";
  for (uint8_t i = 0; i < ZONE_BYTE_COUNT; i++) {
    if (pkt_.get_payload_byte(PLINDEX_ZONE_MASK) & (1 << i)) {
      result += " Z" + std::to_string(i + 1) + ":" + (pkt_.get_payload_byte(PLINDEX_ZONE_OFFSET + i) ? "ON" : "OFF");
    }
  }
  return result;
}

std::string ZoneGetResponsePacket::to_string() const {
  std::string result = "Zone Get Response: " + Packet::to_string() + "\n " + CONSOLE_COLOR_PURPLE + "Zones:";
  for (uint8_t i = 0; i < ZONE_BYTE_COUNT; i++) {
    result += " Z" + std::to_string(i + 1) + ":" + (get_zone_active(i) ? "ON" : "OFF");
  }
  return result;
}

}  // namespace itp_packet
