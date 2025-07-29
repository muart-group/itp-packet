#pragma once

#include "./packet.h"

namespace itp_packet {
class SettingsSetRequestPacket : public Packet {
  static const int PLINDEX_POWER = 3;
  static const int PLINDEX_MODE = 4;
  static const int PLINDEX_TARGET_TEMPERATURE_CODE = 5;
  static const int PLINDEX_FAN = 6;
  static const int PLINDEX_VANE = 7;
  static const int PLINDEX_HORIZONTAL_VANE = 13;
  static const int PLINDEX_TARGET_TEMPERATURE = 14;

  enum SettingFlag : uint8_t {
    SF_POWER = 0x01,
    SF_MODE = 0x02,
    SF_TARGET_TEMPERATURE = 0x04,
    SF_FAN = 0x08,
    SF_VANE = 0x10
  };

  enum SettingFlag2 : uint8_t {
    SF2_HORIZONTAL_VANE = 0x01,
  };

 public:
  enum ModeByte : uint8_t {
    MODE_BYTE_HEAT = 0x01,
    MODE_BYTE_DRY = 0x02,
    MODE_BYTE_COOL = 0x03,
    MODE_BYTE_FAN = 0x07,
    MODE_BYTE_AUTO = 0x08,
  };

  enum FanByte : uint8_t {
    FAN_AUTO = 0x00,
    FAN_QUIET = 0x01,
    FAN_1 = 0x02,
    FAN_2 = 0x03,
    FAN_3 = 0x05,
    FAN_4 = 0x06,
  };

  enum VaneByte : uint8_t {
    VANE_AUTO = 0x00,
    VANE_1 = 0x01,
    VANE_2 = 0x02,
    VANE_3 = 0x03,
    VANE_4 = 0x04,
    VANE_5 = 0x05,
    VANE_SWING = 0x07,
  };

  enum HorizontalVaneByte : uint8_t {
    HV_AUTO = 0x00,
    HV_LEFT_FULL = 0x01,
    HV_LEFT = 0x02,
    HV_CENTER = 0x03,
    HV_RIGHT = 0x04,
    HV_RIGHT_FULL = 0x05,
    HV_SPLIT = 0x08,
    HV_SWING = 0x0c,
  };

  SettingsSetRequestPacket() : Packet(RawPacket(PacketType::SET_REQUEST, 16)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(SetCommand::SETTINGS));
  }
  using Packet::Packet;

  uint8_t get_power() const { return pkt_.get_payload_byte(PLINDEX_POWER); }
  ModeByte get_mode() const { return (ModeByte) pkt_.get_payload_byte(PLINDEX_MODE); }
  FanByte get_fan() const { return (FanByte) pkt_.get_payload_byte(PLINDEX_FAN); }
  VaneByte get_vane() const { return (VaneByte) pkt_.get_payload_byte(PLINDEX_VANE); }
  HorizontalVaneByte get_horizontal_vane() const {
    return (HorizontalVaneByte) (pkt_.get_payload_byte(PLINDEX_HORIZONTAL_VANE) & 0x7F);
  }
  bool get_horizontal_vane_msb() const { return pkt_.get_payload_byte(PLINDEX_HORIZONTAL_VANE) & 0x80; }

  float get_target_temp() const;

  SettingsSetRequestPacket &set_power(bool is_on);
  SettingsSetRequestPacket &set_mode(ModeByte mode);
  SettingsSetRequestPacket &set_target_temperature(float temperature_degrees_c);
  SettingsSetRequestPacket &set_fan(FanByte fan);
  SettingsSetRequestPacket &set_vane(VaneByte vane);
  SettingsSetRequestPacket &set_horizontal_vane(HorizontalVaneByte horizontal_vane);

  std::string to_string() const override;

 private:
  void add_settings_flag_(SettingFlag flag_to_add);
  void add_settings_flag2_(SettingFlag2 flag2_to_add);
};

class RemoteTemperatureSetRequestPacket : public Packet {
  static const uint8_t PLINDEX_LEGACY_REMOTE_TEMPERATURE = 2;
  static const uint8_t PLINDEX_REMOTE_TEMPERATURE = 3;

 public:
  RemoteTemperatureSetRequestPacket() : Packet(RawPacket(PacketType::SET_REQUEST, 4)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(SetCommand::REMOTE_TEMPERATURE));
  }
  using Packet::Packet;

  float get_remote_temperature() const;

  RemoteTemperatureSetRequestPacket &set_remote_temperature(float temperature_degrees_c);
  RemoteTemperatureSetRequestPacket &use_internal_temperature();

  std::string to_string() const override;
};

class SetResponsePacket : public Packet {
  using Packet::Packet;

 public:
  SetResponsePacket() : Packet(RawPacket(PacketType::SET_RESPONSE, 16)) {}

  uint8_t get_result_code() const { return pkt_.get_payload_byte(0); }
  bool is_successful() const { return get_result_code() == 0; }
};

class SetRunStatePacket : public Packet {
  // bytes 1 and 2 are update flags
  static const uint8_t PLINDEX_FILTER_RESET = 3;

  using Packet::Packet;

 public:
  SetRunStatePacket() : Packet(RawPacket(PacketType::SET_REQUEST, 10)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(SetCommand::RUN_STATE));
  }

  bool get_filter_reset() { return pkt_.get_payload_byte(PLINDEX_FILTER_RESET) != 0; };
  SetRunStatePacket &set_filter_reset(bool do_reset);
};
}  // namespace itp_packet