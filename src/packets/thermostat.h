#pragma once

#include "itp_packet.h"

namespace itp_packet {
class ThermostatSensorStatusPacket : public Packet {
  using Packet::Packet;

 public:
  enum ThermostatBatteryState : uint8_t {
    THERMOSTAT_BATTERY_OK = 0x00,
    THERMOSTAT_BATTERY_LOW = 0x01,
    THERMOSTAT_BATTERY_CRITICAL = 0x02,
    THERMOSTAT_BATTERY_REPLACE = 0x03,
    THERMOSTAT_BATTERY_UNKNOWN = 0x04,
  };

  ThermostatSensorStatusPacket() : Packet(RawPacket(PacketType::SET_REQUEST, 16)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(SetCommand::THERMOSTAT_SENSOR_STATUS));
  }

  uint8_t get_indoor_humidity_percent() const { return pkt_.get_payload_byte(5); }
  ThermostatBatteryState get_thermostat_battery_state() const {
    return (ThermostatBatteryState) pkt_.get_payload_byte(6);
  }
  uint8_t get_sensor_flags() const { return pkt_.get_payload_byte(7); }

  std::string to_string() const override;
};

// Sent by MHK2 but with no response; defined to allow setResponseExpected(false)
class ThermostatHelloPacket : public Packet {
  using Packet::Packet;

 public:
  ThermostatHelloPacket() : Packet(RawPacket(PacketType::SET_REQUEST, 16)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(SetCommand::THERMOSTAT_HELLO));
  }

  std::string get_thermostat_model() const;
  std::string get_thermostat_serial() const;
  std::string get_thermostat_version_string() const;

  std::string to_string() const override;
};

class ThermostatStateUploadPacket : public Packet {
  // Packet 0x41 - AG 0xA8

  static const uint8_t PLINDEX_THERMOSTAT_TIMESTAMP = 2;
  static const uint8_t PLINDEX_AUTO_MODE = 7;
  static const uint8_t PLINDEX_HEAT_SETPOINT = 8;
  static const uint8_t PLINDEX_COOL_SETPOINT = 9;

  enum TSStateSyncFlags : uint8_t {
    TSSF_TIMESTAMP = 0x01,
    TSSF_AUTO_MODE = 0x04,
    TSSF_HEAT_SETPOINT = 0x08,
    TSSF_COOL_SETPOINT = 0x10,
  };

  using Packet::Packet;

 public:
  ThermostatStateUploadPacket() : Packet(RawPacket(PacketType::SET_REQUEST, 16)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(SetCommand::THERMOSTAT_STATE_UPLOAD));
  }

  struct tm get_thermostat_timestamp() const;
  uint8_t get_auto_mode() const;
  float get_heat_setpoint() const;
  float get_cool_setpoint() const;

  std::string to_string() const override;
};

class ThermostatStateDownloadResponsePacket : public Packet {
  static const uint8_t PLINDEX_ADAPTER_TIMESTAMP = 1;
  static const uint8_t PLINDEX_AUTO_MODE = 6;
  static const uint8_t PLINDEX_HEAT_SETPOINT = 7;
  static const uint8_t PLINDEX_COOL_SETPOINT = 8;

  using Packet::Packet;

 public:
  ThermostatStateDownloadResponsePacket() : Packet(RawPacket(PacketType::GET_RESPONSE, 16)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(GetCommand::THERMOSTAT_STATE_DOWNLOAD));
  }

  ThermostatStateDownloadResponsePacket &set_timestamp(time_t ts);
  ThermostatStateDownloadResponsePacket &set_auto_mode(bool is_auto);
  ThermostatStateDownloadResponsePacket &set_heat_setpoint(float high_temp);
  ThermostatStateDownloadResponsePacket &set_cool_setpoint(float low_temp);
};

class ThermostatAASetRequestPacket : public Packet {
  using Packet::Packet;

 public:
  ThermostatAASetRequestPacket() : Packet(RawPacket(PacketType::SET_REQUEST, 16)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(SetCommand::THERMOSTAT_SET_AA));
  }
};

class ThermostatABGetResponsePacket : public Packet {
  using Packet::Packet;

 public:
  ThermostatABGetResponsePacket() : Packet(RawPacket(PacketType::GET_RESPONSE, 16)) {
    pkt_.set_payload_byte(0, static_cast<uint8_t>(GetCommand::THERMOSTAT_GET_AB));
    pkt_.set_payload_byte(1, 1);
  }
};
}  // namespace itp_packet