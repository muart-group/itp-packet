#pragma once

#include "./packet.h"

namespace itp_packet {

class GetRequestPacket : public Packet {
 public:
  static GetRequestPacket &get_settings_instance() {
    static GetRequestPacket instance = GetRequestPacket(GetCommand::SETTINGS);
    return instance;
  }
  static GetRequestPacket &get_current_temp_instance() {
    static GetRequestPacket instance = GetRequestPacket(GetCommand::CURRENT_TEMP);
    return instance;
  }
  static GetRequestPacket &get_status_instance() {
    static GetRequestPacket instance = GetRequestPacket(GetCommand::STATUS);
    return instance;
  }
  static GetRequestPacket &get_runstate_instance() {
    static GetRequestPacket instance = GetRequestPacket(GetCommand::RUN_STATE);
    return instance;
  }
  static GetRequestPacket &get_error_info_instance() {
    static GetRequestPacket instance = GetRequestPacket(GetCommand::ERROR_INFO);
    return instance;
  }
  static GetRequestPacket &get_functions_1_instance() {
    static GetRequestPacket instance = GetRequestPacket(GetCommand::FUNCTIONS_1);
    return instance;
  }
  static GetRequestPacket &get_functions_2_instance() {
    static GetRequestPacket instance = GetRequestPacket(GetCommand::FUNCTIONS_2);
    return instance;
  }
  using Packet::Packet;

  GetCommand get_requested_command() const { return (GetCommand) pkt_.get_payload_byte(0); }

  std::string to_string() const override;

 private:
  GetRequestPacket(GetCommand get_command) : Packet(RawPacket(PacketType::GET_REQUEST, 1)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(get_command));
  }
};

class SettingsGetResponsePacket : public Packet {
  static const int PLINDEX_POWER = 3;
  static const int PLINDEX_MODE = 4;
  static const int PLINDEX_TARGETTEMP_LEGACY = 5;
  static const int PLINDEX_FAN = 6;
  static const int PLINDEX_VANE = 7;
  static const int PLINDEX_PROHIBITFLAGS = 8;
  static const int PLINDEX_HVANE = 10;
  static const int PLINDEX_TARGETTEMP = 11;
  using Packet::Packet;

 public:
  uint8_t get_power() const { return pkt_.get_payload_byte(PLINDEX_POWER); }
  uint8_t get_mode() const { return pkt_.get_payload_byte(PLINDEX_MODE); }
  const uint8_t get_fan() const { return pkt_.get_payload_byte(PLINDEX_FAN); }
  uint8_t get_vane() const { return pkt_.get_payload_byte(PLINDEX_VANE); }
  bool locked_power() const { return pkt_.get_payload_byte(PLINDEX_PROHIBITFLAGS) & 0x01; }
  bool locked_mode() const { return pkt_.get_payload_byte(PLINDEX_PROHIBITFLAGS) & 0x02; }
  bool locked_temp() const { return pkt_.get_payload_byte(PLINDEX_PROHIBITFLAGS) & 0x04; }
  uint8_t get_horizontal_vane() const { return pkt_.get_payload_byte(PLINDEX_HVANE) & 0x7F; }
  bool get_horizontal_vane_msb() const { return pkt_.get_payload_byte(PLINDEX_HVANE) & 0x80; }

  float get_target_temp() const;

  bool is_i_see_enabled() const;

  std::string to_string() const override;
};

class CurrentTempGetResponsePacket : public Packet {
  static const int PLINDEX_CURRENTTEMP_LEGACY = 3;
  static const int PLINDEX_OUTDOORTEMP = 5;
  static const int PLINDEX_CURRENTTEMP = 6;
  static const int PLINDEX_RUNTIME = 11;  // to 13.
  using Packet::Packet;

 public:
  float get_current_temp() const;
  // Returns outdoor temperature or NAN if unsupported
  float get_outdoor_temp() const;
  // Returns lifetime runtime minutes of unit
  uint32_t get_runtime_minutes() const;
  std::string to_string() const override;
};

class StatusGetResponsePacket : public Packet {
  static const int PLINDEX_COMPRESSOR_FREQUENCY = 3;
  static const int PLINDEX_OPERATING = 4;
  static const int PLINDEX_INPUT_WATTS = 5;   // and 6
  static const int PLINDEX_LIFETIME_KWH = 7;  // and 8

  using Packet::Packet;

 public:
  uint8_t get_compressor_frequency() const { return pkt_.get_payload_byte(PLINDEX_COMPRESSOR_FREQUENCY); }
  bool get_operating() const { return pkt_.get_payload_byte(PLINDEX_OPERATING); }
  uint16_t get_input_watts() const {
    return pkt_.get_payload_byte(PLINDEX_INPUT_WATTS) << 8 | pkt_.get_payload_byte(PLINDEX_INPUT_WATTS + 1);
  }
  float get_lifetime_kwh() const;
  std::string to_string() const override;
};

class RunStateGetResponsePacket : public Packet {
  static const int PLINDEX_STATUSFLAGS = 3;
  static const int PLINDEX_ACTUALFAN = 4;
  static const int PLINDEX_AUTOMODE = 5;
  using Packet::Packet;

 public:
  bool service_filter() const { return pkt_.get_payload_byte(PLINDEX_STATUSFLAGS) & 0x01; }
  bool in_defrost() const { return pkt_.get_payload_byte(PLINDEX_STATUSFLAGS) & 0x02; }
  bool in_preheat() const { return pkt_.get_payload_byte(PLINDEX_STATUSFLAGS) & 0x04; }
  bool in_standby() const { return pkt_.get_payload_byte(PLINDEX_STATUSFLAGS) & 0x08; }
  uint8_t get_actual_fan_speed() const { return pkt_.get_payload_byte(PLINDEX_ACTUALFAN); }
  uint8_t get_auto_mode() const { return pkt_.get_payload_byte(PLINDEX_AUTOMODE); }
  std::string to_string() const override;
};

class ErrorStateGetResponsePacket : public Packet {
  using Packet::Packet;

 public:
  uint16_t get_error_code() const { return pkt_.get_payload_byte(4) << 8 | pkt_.get_payload_byte(5); }
  uint8_t get_raw_short_code() const { return pkt_.get_payload_byte(6); }
  std::string get_short_code() const;

  bool error_present() const { return get_error_code() != 0x8000 || get_raw_short_code() != 0x00; }

  std::string to_string() const override;
};

class Functions1GetResponsePacket : public Packet {
  using Packet::Packet;

 public:
  std::string to_string() const override;
};

class Functions2GetResponsePacket : public Packet {
  using Packet::Packet;

 public:
  std::string to_string() const override;
};
}  // namespace itp_packet