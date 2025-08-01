#include "capabilities.h"

namespace itp_packet {

uint8_t CapabilitiesResponsePacket::get_supported_fan_speeds() const {
  uint8_t raw_value = ((pkt_.get_payload_byte(7) & 0x10) >> 2) + ((pkt_.get_payload_byte(8) & 0x08) >> 2) +
                      ((pkt_.get_payload_byte(9) & 0x02) >> 1);

  switch (raw_value) {
    case 1:
    case 2:
    case 4:
      return raw_value;
    case 0:
      return 3;
    case 6:
      return 5;

    default:
      // TODO: ESP_LOGW(PACKETS_TAG, "Unexpected supported fan speeds: %i", raw_value);
      return 0;  // TODO: Depending on how this is used, it might be more useful to just return 3 and hope for the best
  }
}

std::string CapabilitiesResponsePacket::to_string() const {
  return (
      "Identify Base Capabilities Response: " + Packet::to_string() + CONSOLE_COLOR_PURPLE +
      "\n HeatDisabled:" + (is_heat_disabled() ? "Yes" : "No") + " SupportsVane:" + (supports_vane() ? "Yes" : "No") +
      " SupportsVaneSwing:" + (supports_vane_swing() ? "Yes" : "No")

      + " DryDisabled:" + (is_dry_disabled() ? "Yes" : "No") + " FanDisabled:" + (is_fan_disabled() ? "Yes" : "No") +
      " ExtTempRange:" + (has_extended_temperature_range() ? "Yes" : "No") +
      " AutoFanDisabled:" + (auto_fan_speed_disabled() ? "Yes" : "No") +
      " InstallerSettings:" + (supports_installer_settings() ? "Yes" : "No") +
      " TestMode:" + (supports_test_mode() ? "Yes" : "No") + " DryTemp:" + (supports_dry_temperature() ? "Yes" : "No")

      + " StatusDisplay:" + (has_status_display() ? "Yes" : "No")

      + "\n CoolDrySetpoint:" + std::to_string(get_min_cool_dry_setpoint()) + "/" +
      std::to_string(get_max_cool_dry_setpoint()) + " HeatSetpoint:" + std::to_string(get_min_heating_setpoint()) +
      "/" + std::to_string(get_max_heating_setpoint()) + " AutoSetpoint:" + std::to_string(get_min_auto_setpoint()) +
      "/" + std::to_string(get_max_auto_setpoint()) + " FanSpeeds:" + std::to_string(get_supported_fan_speeds()));
}

}  // namespace itp_packet