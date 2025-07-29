#pragma once

#include "itp_packet.h"

namespace itp_packet {
class CapabilitiesRequestPacket : public Packet {
 public:
  static CapabilitiesRequestPacket &instance() {
    static CapabilitiesRequestPacket instance;
    return instance;
  }
  using Packet::Packet;

 private:
  CapabilitiesRequestPacket() : Packet(RawPacket(PacketType::IDENTIFY_REQUEST, 1)) { pkt_.set_payload_byte(0, 0xc9); }
};

class CapabilitiesResponsePacket : public Packet {
  using Packet::Packet;

 public:
  // Byte 7
  bool is_heat_disabled() const { return pkt_.get_payload_byte(7) & 0x02; }
  bool supports_vane() const { return pkt_.get_payload_byte(7) & 0x20; }
  bool supports_vane_swing() const { return pkt_.get_payload_byte(7) & 0x40; }

  // Byte 8
  bool is_dry_disabled() const { return pkt_.get_payload_byte(8) & 0x01; }
  bool is_fan_disabled() const { return pkt_.get_payload_byte(8) & 0x02; }
  bool has_extended_temperature_range() const { return pkt_.get_payload_byte(8) & 0x04; }
  bool auto_fan_speed_disabled() const { return pkt_.get_payload_byte(8) & 0x10; }
  bool supports_installer_settings() const { return pkt_.get_payload_byte(8) & 0x20; }
  bool supports_test_mode() const { return pkt_.get_payload_byte(8) & 0x40; }
  bool supports_dry_temperature() const { return pkt_.get_payload_byte(8) & 0x80; }

  // Byte 9
  bool has_status_display() const { return pkt_.get_payload_byte(9) & 0x01; }

  // Bytes 10-15
  float get_min_cool_dry_setpoint() const { return ITPUtils::temp_scale_a_to_deg_c(pkt_.get_payload_byte(10)); }
  float get_max_cool_dry_setpoint() const { return ITPUtils::temp_scale_a_to_deg_c(pkt_.get_payload_byte(11)); }
  float get_min_heating_setpoint() const { return ITPUtils::temp_scale_a_to_deg_c(pkt_.get_payload_byte(12)); }
  float get_max_heating_setpoint() const { return ITPUtils::temp_scale_a_to_deg_c(pkt_.get_payload_byte(13)); }
  float get_min_auto_setpoint() const { return ITPUtils::temp_scale_a_to_deg_c(pkt_.get_payload_byte(14)); }
  float get_max_auto_setpoint() const { return ITPUtils::temp_scale_a_to_deg_c(pkt_.get_payload_byte(15)); }

  // Things that have to exist, but we don't know where yet.
  bool supports_h_vane() const { return true; }

  // Fan Speeds TODO: Probably move this to .cpp?
  uint8_t get_supported_fan_speeds() const;

  std::string to_string() const override;
};

}  // namespace itp_packet