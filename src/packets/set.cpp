#include "set.h"

namespace itp_packet {
std::string RemoteTemperatureSetRequestPacket::to_string() const {
  return ("Remote Temp Set Request: " + Packet::to_string() + CONSOLE_COLOR_PURPLE +
          "\n Temp:" + std::to_string(get_remote_temperature()));
}
std::string SettingsSetRequestPacket::to_string() const {
  uint8_t flags = get_flags();
  uint8_t flags2 = get_flags_2();

  std::string result = "Settings Set Request: " + Packet::to_string() + CONSOLE_COLOR_PURPLE +
                       "\n Flags: " + ITPUtils::format_hex(flags2) + ITPUtils::format_hex(flags) + " =>";

  if (flags & SettingFlag::SF_POWER)
    result += " Power: " + std::to_string(get_power());
  if (flags & SettingFlag::SF_MODE)
    result += " Mode: " + std::to_string(get_mode());
  if (flags & SettingFlag::SF_TARGET_TEMPERATURE)
    result += " TargetTemp: " + std::to_string(get_target_temp());
  if (flags & SettingFlag::SF_FAN)
    result += " Fan: " + std::to_string(get_fan());
  if (flags & SettingFlag::SF_VANE)
    result += " Vane: " + std::to_string(get_vane());

  if (flags2 & SettingFlag2::SF2_HORIZONTAL_VANE)
    result += " HVane: " + std::to_string(get_horizontal_vane()) + (get_horizontal_vane_msb() ? " (MSB Set)" : "");

  return result;
}

void SettingsSetRequestPacket::add_settings_flag_(const SettingFlag flag_to_add) { add_flag(flag_to_add); }

void SettingsSetRequestPacket::add_settings_flag2_(const SettingFlag2 flag2_to_add) { add_flag2(flag2_to_add); }

SettingsSetRequestPacket &SettingsSetRequestPacket::set_power(const bool is_on) {
  pkt_.set_payload_byte(PLINDEX_POWER, is_on ? 0x01 : 0x00);
  add_settings_flag_(SF_POWER);
  return *this;
}

SettingsSetRequestPacket &SettingsSetRequestPacket::set_mode(const ModeByte mode) {
  pkt_.set_payload_byte(PLINDEX_MODE, mode);
  add_settings_flag_(SF_MODE);
  return *this;
}

SettingsSetRequestPacket &SettingsSetRequestPacket::set_target_temperature(const float temperature_degrees_c) {
  if (temperature_degrees_c < 63.5 && temperature_degrees_c > -64.0) {
    pkt_.set_payload_byte(PLINDEX_TARGET_TEMPERATURE, ITPUtils::deg_c_to_temp_scale_a(temperature_degrees_c));
    pkt_.set_payload_byte(PLINDEX_TARGET_TEMPERATURE_CODE,
                          ITPUtils::deg_c_to_legacy_target_temp(temperature_degrees_c));

    // TODO: while spawning a warning here is fine, we should (a) only actually send that warning if the system can't
    //       support this setpoint, and (b) clamp the setpoint to the known-acceptable values.
    // The utility class will already clamp this for us, so we only need to worry about the warning.
    if (temperature_degrees_c < 16 || temperature_degrees_c > 31.5) {
      // TODO: ESP_LOGW(PTAG, "Target temp %f is out of range for the legacy temp scale. This may be a problem on older
      // units.",
      //          temperature_degrees_c);
    }

    add_settings_flag_(SF_TARGET_TEMPERATURE);
  } else {
    // TODO: ESP_LOGW(PTAG, "Target temp %f is outside valid range - target temperature not set!",
    // temperature_degrees_c);
  }

  return *this;
}
SettingsSetRequestPacket &SettingsSetRequestPacket::set_fan(const FanByte fan) {
  pkt_.set_payload_byte(PLINDEX_FAN, fan);
  add_settings_flag_(SF_FAN);
  return *this;
}

SettingsSetRequestPacket &SettingsSetRequestPacket::set_vane(const VaneByte vane) {
  pkt_.set_payload_byte(PLINDEX_VANE, vane);
  add_settings_flag_(SF_VANE);
  return *this;
}

SettingsSetRequestPacket &SettingsSetRequestPacket::set_horizontal_vane(const HorizontalVaneByte horizontal_vane) {
  pkt_.set_payload_byte(PLINDEX_HORIZONTAL_VANE, horizontal_vane);
  add_settings_flag2_(SF2_HORIZONTAL_VANE);
  return *this;
}

float SettingsSetRequestPacket::get_target_temp() const {
  uint8_t enhanced_raw_temp = pkt_.get_payload_byte(PLINDEX_TARGET_TEMPERATURE);

  if (enhanced_raw_temp == 0x00) {
    uint8_t legacy_raw_temp = pkt_.get_payload_byte(PLINDEX_TARGET_TEMPERATURE_CODE);
    return ITPUtils::legacy_target_temp_to_deg_c(legacy_raw_temp);
  }

  return ITPUtils::temp_scale_a_to_deg_c(enhanced_raw_temp);
}

// RemoteTemperatureSetRequestPacket functions

float RemoteTemperatureSetRequestPacket::get_remote_temperature() const {
  uint8_t raw_temp_a = pkt_.get_payload_byte(PLINDEX_REMOTE_TEMPERATURE);

  if (raw_temp_a == 0) {
    uint8_t raw_temp_legacy = pkt_.get_payload_byte(PLINDEX_LEGACY_REMOTE_TEMPERATURE);
    return ITPUtils::legacy_ts_room_temp_to_deg_c(raw_temp_legacy);
  }

  return ITPUtils::temp_scale_a_to_deg_c(raw_temp_a);
}

RemoteTemperatureSetRequestPacket &RemoteTemperatureSetRequestPacket::set_remote_temperature(
    float temperature_degrees_c) {
  if (temperature_degrees_c < 63.5 && temperature_degrees_c > -64.0) {
    pkt_.set_payload_byte(PLINDEX_REMOTE_TEMPERATURE, ITPUtils::deg_c_to_temp_scale_a(temperature_degrees_c));
    pkt_.set_payload_byte(PLINDEX_LEGACY_REMOTE_TEMPERATURE,
                          ITPUtils::deg_c_to_legacy_ts_room_temp(temperature_degrees_c));
    set_flags(0x01);  // Set flags to say we're providing the temperature
  } else {
    // TODO: ESP_LOGW(PTAG, "Remote temp %f is outside valid range.", temperature_degrees_c);
  }
  return *this;
}

bool RemoteTemperatureSetRequestPacket::get_use_internal_temperature() const { return 0x00 == get_flags() & 0x01; }

RemoteTemperatureSetRequestPacket &RemoteTemperatureSetRequestPacket::set_use_internal_temperature(bool use_internal) {
  set_flags(use_internal ? 0x00 : 0x01);  // Set flags to say to use internal temperature
  return *this;
}

// SettingsSetRunStatusPacket functions
SetRunStatePacket &SetRunStatePacket::set_filter_reset(bool do_reset) {
  pkt_.set_payload_byte(PLINDEX_FILTER_RESET, do_reset ? 1 : 0);
  set_flags(0x01);
  return *this;
}
}  // namespace itp_packet