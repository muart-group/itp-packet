#include "thermostat.h"

namespace itp_packet {
std::string ThermostatSensorStatusPacket::to_string() const {
  return ("Thermostat Sensor Status: " + Packet::to_string() + CONSOLE_COLOR_PURPLE +
          "\n Indoor RH: " + std::to_string(get_indoor_humidity_percent()) + "%" +
          "  MHK Battery: " + THERMOSTAT_BATTERY_STATE_NAMES[get_thermostat_battery_state()] + "(" +
          std::to_string(get_thermostat_battery_state()) + ")" +
          "  Sensor Flags: " + std::to_string(get_sensor_flags()));
}

std::string ThermostatHelloPacket::to_string() const {
  return ("Thermostat Hello: " + Packet::to_string() + CONSOLE_COLOR_PURPLE + "\n Model: " + get_thermostat_model() +
          " Serial: " + get_thermostat_serial() + " Version: " + get_thermostat_version_string());
}

std::string ThermostatStateUploadPacket::to_string() const {
  uint8_t flags = get_flags();

  std::string result = "Thermostat Sync " + Packet::to_string() + CONSOLE_COLOR_PURPLE +
                       "\n Flags: " + ITPUtils::format_hex(flags) + " =>";

  if (flags & TSSF_TIMESTAMP) {
    struct tm ts_timestamp = get_thermostat_timestamp();
    char ts_chars[32];  // Buffer to store the formatted string
    strftime(ts_chars, sizeof(ts_chars), "%Y-%m-%d %H:%M:%S", &ts_timestamp);

    result += " TS Time: " + (std::string) ts_chars;
  }

  if (flags & TSSF_AUTO_MODE)
    result += " AutoMode: " + std::to_string(get_auto_mode());
  if (flags & TSSF_HEAT_SETPOINT)
    result += " HeatSetpoint: " + std::to_string(get_heat_setpoint());
  if (flags & TSSF_COOL_SETPOINT)
    result += " CoolSetpoint: " + std::to_string(get_cool_setpoint());

  return result;
}

std::string ThermostatHelloPacket::get_thermostat_model() const {
  return ITPUtils::decode_n_bit_string((pkt_.get_payload_bytes(1)), 4, 6);
}

std::string ThermostatHelloPacket::get_thermostat_serial() const {
  return ITPUtils::decode_n_bit_string((pkt_.get_payload_bytes(4)), 12, 6);
}

std::string ThermostatHelloPacket::get_thermostat_version_string() const {
  char buf[16];
  sprintf(buf, "%02d.%02d.%02d", pkt_.get_payload_byte(13), pkt_.get_payload_byte(14), pkt_.get_payload_byte(15));

  return buf;
}

// ThermostatStateUploadPacket functions
struct tm ThermostatStateUploadPacket::get_thermostat_timestamp() const {
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

float ThermostatStateUploadPacket::get_heat_setpoint() const {
  uint8_t enhanced_raw_temp = pkt_.get_payload_byte(PLINDEX_HEAT_SETPOINT);
  return ITPUtils::temp_scale_a_to_deg_c(enhanced_raw_temp);
}

float ThermostatStateUploadPacket::get_cool_setpoint() const {
  uint8_t enhanced_raw_temp = pkt_.get_payload_byte(PLINDEX_COOL_SETPOINT);
  return ITPUtils::temp_scale_a_to_deg_c(enhanced_raw_temp);
}

// ThermostatStateDownloadResponsePacket functions
ThermostatStateDownloadResponsePacket &ThermostatStateDownloadResponsePacket::set_timestamp(time_t ts) {
  // int32_t encoded_timestamp = ((ts.year - 2017) << 26) | (ts.month << 22) | (ts.day_of_month << 17) | (ts.hour << 12)
  // |
  //                             (ts.minute << 6) | (ts.second);
  int32_t encoded_timestamp = (int32_t) ts;

  int32_t swapped_timestamp = __builtin_bswap32(encoded_timestamp);

  pkt_.set_payload_bytes(PLINDEX_ADAPTER_TIMESTAMP, &swapped_timestamp, 4);
  pkt_.set_payload_byte(10, 0x07);  // ???

  return *this;
}

ThermostatStateDownloadResponsePacket &ThermostatStateDownloadResponsePacket::set_auto_mode(bool is_auto) {
  pkt_.set_payload_byte(PLINDEX_AUTO_MODE, is_auto ? 0x01 : 0x00);
  return *this;
}

ThermostatStateDownloadResponsePacket &ThermostatStateDownloadResponsePacket::set_heat_setpoint(float high_temp) {
  uint8_t temp_a = high_temp != NAN ? ITPUtils::deg_c_to_temp_scale_a(high_temp) : 0x00;

  pkt_.set_payload_byte(PLINDEX_HEAT_SETPOINT, temp_a);
  return *this;
}

ThermostatStateDownloadResponsePacket &ThermostatStateDownloadResponsePacket::set_cool_setpoint(float low_temp) {
  uint8_t temp_a = low_temp != NAN ? ITPUtils::deg_c_to_temp_scale_a(low_temp) : 0x00;

  pkt_.set_payload_byte(PLINDEX_COOL_SETPOINT, temp_a);
  return *this;
}
}  // namespace itp_packet