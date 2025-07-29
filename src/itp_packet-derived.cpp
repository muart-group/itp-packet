#include <time.h>

#include "itp_packet.h"
#include "itp_utils.h"

namespace itp_packet
{

  // Packet to_strings()

  std::string ConnectRequestPacket::to_string() const { return ("Connect Request: " + Packet::to_string()); }
  std::string ConnectResponsePacket::to_string() const { return ("Connect Response: " + Packet::to_string()); }
  std::string CapabilitiesResponsePacket::to_string() const
  {
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
  std::string IdentifyCDResponsePacket::to_string() const { return "Identify CD Response: " + Packet::to_string(); }
  std::string RemoteTemperatureSetRequestPacket::to_string() const
  {
    return ("Remote Temp Set Request: " + Packet::to_string() + CONSOLE_COLOR_PURPLE +
            "\n Temp:" + std::to_string(get_remote_temperature()));
  }

  std::string ThermostatSensorStatusPacket::to_string() const
  {
    return ("Thermostat Sensor Status: " + Packet::to_string() + CONSOLE_COLOR_PURPLE +
            "\n Indoor RH: " + std::to_string(get_indoor_humidity_percent()) + "%" +
            "  MHK Battery: " + THERMOSTAT_BATTERY_STATE_NAMES[get_thermostat_battery_state()] + "(" +
            std::to_string(get_thermostat_battery_state()) + ")" +
            "  Sensor Flags: " + std::to_string(get_sensor_flags()));
  }

  std::string ThermostatHelloPacket::to_string() const
  {
    return ("Thermostat Hello: " + Packet::to_string() + CONSOLE_COLOR_PURPLE + "\n Model: " + get_thermostat_model() +
            " Serial: " + get_thermostat_serial() + " Version: " + get_thermostat_version_string());
  }

  std::string ThermostatStateUploadPacket::to_string() const
  {
    uint8_t flags = get_flags();

    std::string result =
        "Thermostat Sync " + Packet::to_string() + CONSOLE_COLOR_PURPLE + "\n Flags: " + ITPUtils::format_hex(flags) + " =>";

    if (flags & TSSF_TIMESTAMP)
    {
      struct tm ts_timestamp = get_thermostat_timestamp();
      char ts_chars[32]; // Buffer to store the formatted string
      strftime(ts_chars, sizeof(ts_chars), "%Y-%m-%d %H:%M:%S", &ts_timestamp);

      result += " TS Time: " + (std::string)ts_chars;
    }

    if (flags & TSSF_AUTO_MODE)
      result += " AutoMode: " + std::to_string(get_auto_mode());
    if (flags & TSSF_HEAT_SETPOINT)
      result += " HeatSetpoint: " + std::to_string(get_heat_setpoint());
    if (flags & TSSF_COOL_SETPOINT)
      result += " CoolSetpoint: " + std::to_string(get_cool_setpoint());

    return result;
  }

  std::string GetRequestPacket::to_string() const
  {
    return ("Get Request: " + Packet::to_string() + CONSOLE_COLOR_PURPLE +
            "\n CommandID: " + ITPUtils::format_hex((uint8_t)get_requested_command()));
  }

  std::string SettingsSetRequestPacket::to_string() const
  {
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

  // TODO: Are there function implementations for packets in the .h file? (Yes)  Should they be here?

  // SettingsSetRequestPacket functions

  void SettingsSetRequestPacket::add_settings_flag_(const SettingFlag flag_to_add) { add_flag(flag_to_add); }

  void SettingsSetRequestPacket::add_settings_flag2_(const SettingFlag2 flag2_to_add) { add_flag2(flag2_to_add); }

  SettingsSetRequestPacket &SettingsSetRequestPacket::set_power(const bool is_on)
  {
    pkt_.set_payload_byte(PLINDEX_POWER, is_on ? 0x01 : 0x00);
    add_settings_flag_(SF_POWER);
    return *this;
  }

  SettingsSetRequestPacket &SettingsSetRequestPacket::set_mode(const ModeByte mode)
  {
    pkt_.set_payload_byte(PLINDEX_MODE, mode);
    add_settings_flag_(SF_MODE);
    return *this;
  }

  SettingsSetRequestPacket &SettingsSetRequestPacket::set_target_temperature(const float temperature_degrees_c)
  {
    if (temperature_degrees_c < 63.5 && temperature_degrees_c > -64.0)
    {
      pkt_.set_payload_byte(PLINDEX_TARGET_TEMPERATURE, ITPUtils::deg_c_to_temp_scale_a(temperature_degrees_c));
      pkt_.set_payload_byte(PLINDEX_TARGET_TEMPERATURE_CODE,
                            ITPUtils::deg_c_to_legacy_target_temp(temperature_degrees_c));

      // TODO: while spawning a warning here is fine, we should (a) only actually send that warning if the system can't
      //       support this setpoint, and (b) clamp the setpoint to the known-acceptable values.
      // The utility class will already clamp this for us, so we only need to worry about the warning.
      if (temperature_degrees_c < 16 || temperature_degrees_c > 31.5)
      {
        // TODO: ESP_LOGW(PTAG, "Target temp %f is out of range for the legacy temp scale. This may be a problem on older units.",
        //          temperature_degrees_c);
      }

      add_settings_flag_(SF_TARGET_TEMPERATURE);
    }
    else
    {
      // TODO: ESP_LOGW(PTAG, "Target temp %f is outside valid range - target temperature not set!", temperature_degrees_c);
    }

    return *this;
  }
  SettingsSetRequestPacket &SettingsSetRequestPacket::set_fan(const FanByte fan)
  {
    pkt_.set_payload_byte(PLINDEX_FAN, fan);
    add_settings_flag_(SF_FAN);
    return *this;
  }

  SettingsSetRequestPacket &SettingsSetRequestPacket::set_vane(const VaneByte vane)
  {
    pkt_.set_payload_byte(PLINDEX_VANE, vane);
    add_settings_flag_(SF_VANE);
    return *this;
  }

  SettingsSetRequestPacket &SettingsSetRequestPacket::set_horizontal_vane(const HorizontalVaneByte horizontal_vane)
  {
    pkt_.set_payload_byte(PLINDEX_HORIZONTAL_VANE, horizontal_vane);
    add_settings_flag2_(SF2_HORIZONTAL_VANE);
    return *this;
  }

  float SettingsSetRequestPacket::get_target_temp() const
  {
    uint8_t enhanced_raw_temp = pkt_.get_payload_byte(PLINDEX_TARGET_TEMPERATURE);

    if (enhanced_raw_temp == 0x00)
    {
      uint8_t legacy_raw_temp = pkt_.get_payload_byte(PLINDEX_TARGET_TEMPERATURE_CODE);
      return ITPUtils::legacy_target_temp_to_deg_c(legacy_raw_temp);
    }

    return ITPUtils::temp_scale_a_to_deg_c(enhanced_raw_temp);
  }

  // RemoteTemperatureSetRequestPacket functions

  float RemoteTemperatureSetRequestPacket::get_remote_temperature() const
  {
    uint8_t raw_temp_a = pkt_.get_payload_byte(PLINDEX_REMOTE_TEMPERATURE);

    if (raw_temp_a == 0)
    {
      uint8_t raw_temp_legacy = pkt_.get_payload_byte(PLINDEX_LEGACY_REMOTE_TEMPERATURE);
      return ITPUtils::legacy_ts_room_temp_to_deg_c(raw_temp_legacy);
    }

    return ITPUtils::temp_scale_a_to_deg_c(raw_temp_a);
  }

  RemoteTemperatureSetRequestPacket &RemoteTemperatureSetRequestPacket::set_remote_temperature(
      float temperature_degrees_c)
  {
    if (temperature_degrees_c < 63.5 && temperature_degrees_c > -64.0)
    {
      pkt_.set_payload_byte(PLINDEX_REMOTE_TEMPERATURE, ITPUtils::deg_c_to_temp_scale_a(temperature_degrees_c));
      pkt_.set_payload_byte(PLINDEX_LEGACY_REMOTE_TEMPERATURE,
                            ITPUtils::deg_c_to_legacy_ts_room_temp(temperature_degrees_c));
      set_flags(0x01); // Set flags to say we're providing the temperature
    }
    else
    {
      // TODO: ESP_LOGW(PTAG, "Remote temp %f is outside valid range.", temperature_degrees_c);
    }
    return *this;
  }
  RemoteTemperatureSetRequestPacket &RemoteTemperatureSetRequestPacket::use_internal_temperature()
  {
    set_flags(0x00); // Set flags to say to use internal temperature
    return *this;
  }

  // SettingsSetRunStatusPacket functions
  SetRunStatePacket &SetRunStatePacket::set_filter_reset(bool do_reset)
  {
    pkt_.set_payload_byte(PLINDEX_FILTER_RESET, do_reset ? 1 : 0);
    set_flags(0x01);
    return *this;
  }

  // ThermostatHelloPacket functions
  std::string ThermostatHelloPacket::get_thermostat_model() const
  {
    return ITPUtils::decode_n_bit_string((pkt_.get_payload_bytes(1)), 4, 6);
  }

  std::string ThermostatHelloPacket::get_thermostat_serial() const
  {
    return ITPUtils::decode_n_bit_string((pkt_.get_payload_bytes(4)), 12, 6);
  }

  std::string ThermostatHelloPacket::get_thermostat_version_string() const
  {
    char buf[16];
    sprintf(buf, "%02d.%02d.%02d", pkt_.get_payload_byte(13), pkt_.get_payload_byte(14), pkt_.get_payload_byte(15));

    return buf;
  }

  // ThermostatStateUploadPacket functions
  struct tm ThermostatStateUploadPacket::get_thermostat_timestamp() const
  {
    uint32_t raw_timestamp;
    std::memcpy(&raw_timestamp, pkt_.get_payload_bytes(PLINDEX_THERMOSTAT_TIMESTAMP), 4);

    // Incoming data from thermostat is big-endian, ensure that it's still that way for bitwise operations
    raw_timestamp = ITPUtils::to_big_endian(raw_timestamp);

    struct tm return_timestamp;

    return_timestamp.tm_sec = raw_timestamp & 63;
    return_timestamp.tm_min = (raw_timestamp >> 6) & 63;
    return_timestamp.tm_hour = (raw_timestamp >> 12) & 31;
    return_timestamp.tm_mday = (raw_timestamp >> 17) & 31;
    return_timestamp.tm_mon = (raw_timestamp >> 22) & 15;
    return_timestamp.tm_year = (raw_timestamp >> 26) + 2017;

    // out_timestamp->recalc_timestamp_local();
    return return_timestamp;
  }

  uint8_t ThermostatStateUploadPacket::get_auto_mode() const { return pkt_.get_payload_byte(PLINDEX_AUTO_MODE); }

  float ThermostatStateUploadPacket::get_heat_setpoint() const
  {
    uint8_t enhanced_raw_temp = pkt_.get_payload_byte(PLINDEX_HEAT_SETPOINT);
    return ITPUtils::temp_scale_a_to_deg_c(enhanced_raw_temp);
  }

  float ThermostatStateUploadPacket::get_cool_setpoint() const
  {
    uint8_t enhanced_raw_temp = pkt_.get_payload_byte(PLINDEX_COOL_SETPOINT);
    return ITPUtils::temp_scale_a_to_deg_c(enhanced_raw_temp);
  }

  // ThermostatStateDownloadResponsePacket functions
  ThermostatStateDownloadResponsePacket &ThermostatStateDownloadResponsePacket::set_timestamp(time_t ts)
  {
    // int32_t encoded_timestamp = ((ts.year - 2017) << 26) | (ts.month << 22) | (ts.day_of_month << 17) | (ts.hour << 12) |
    //                             (ts.minute << 6) | (ts.second);
    int32_t encoded_timestamp = (int32_t)ts;

    int32_t swapped_timestamp = __builtin_bswap32(encoded_timestamp);

    pkt_.set_payload_bytes(PLINDEX_ADAPTER_TIMESTAMP, &swapped_timestamp, 4);
    pkt_.set_payload_byte(10, 0x07); // ???

    return *this;
  }

  ThermostatStateDownloadResponsePacket &ThermostatStateDownloadResponsePacket::set_auto_mode(bool is_auto)
  {
    pkt_.set_payload_byte(PLINDEX_AUTO_MODE, is_auto ? 0x01 : 0x00);
    return *this;
  }

  ThermostatStateDownloadResponsePacket &ThermostatStateDownloadResponsePacket::set_heat_setpoint(float high_temp)
  {
    uint8_t temp_a = high_temp != NAN ? ITPUtils::deg_c_to_temp_scale_a(high_temp) : 0x00;

    pkt_.set_payload_byte(PLINDEX_HEAT_SETPOINT, temp_a);
    return *this;
  }

  ThermostatStateDownloadResponsePacket &ThermostatStateDownloadResponsePacket::set_cool_setpoint(float low_temp)
  {
    uint8_t temp_a = low_temp != NAN ? ITPUtils::deg_c_to_temp_scale_a(low_temp) : 0x00;

    pkt_.set_payload_byte(PLINDEX_COOL_SETPOINT, temp_a);
    return *this;
  }

  // CapabilitiesResponsePacket functions
  uint8_t CapabilitiesResponsePacket::get_supported_fan_speeds() const
  {
    uint8_t raw_value = ((pkt_.get_payload_byte(7) & 0x10) >> 2) + ((pkt_.get_payload_byte(8) & 0x08) >> 2) +
                        ((pkt_.get_payload_byte(9) & 0x02) >> 1);

    switch (raw_value)
    {
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
      return 0; // TODO: Depending on how this is used, it might be more useful to just return 3 and hope for the best
    }
  }

} // namespace itp_packet
