#pragma once

#include <array>
#include <cstring>
#include <sstream>
#include <string>
#include "rawpacket.h"
#include "itp_utils.h"
#include "packets/packets.h"

namespace itp_packet {
static constexpr char PACKETS_TAG[] = "mitsubishi_itp.packets";

#define CONSOLE_COLOR_NONE "\033[0m"
#define CONSOLE_COLOR_GREEN "\033[0;32m"
#define CONSOLE_COLOR_PURPLE "\033[0;35m"
#define CONSOLE_COLOR_CYAN "\033[0;36m"
#define CONSOLE_COLOR_CYAN_BOLD "\033[1;36m"
#define CONSOLE_COLOR_WHITE "\033[0;37m"

// Defined as constant for use as a Custom Fan Mode
const std::string FAN_MODE_VERYHIGH = "Very High";

// These are named to match with set fan speeds where possible.  "Very Low" is a special speed
// for e.g. preheating or thermal off.
const std::array<std::string, 7> ACTUAL_FAN_SPEED_NAMES = {"Off",  "Very Low",        "Low",  "Medium",
                                                           "High", FAN_MODE_VERYHIGH, "Quiet"};

const std::array<std::string, 5> THERMOSTAT_BATTERY_STATE_NAMES = {"OK", "Low", "Critical", "Replace", "Unknown"};

class PacketProcessor;

// Generic Base Packet wrapper over RawPacket
class Packet {
 public:
  Packet(RawPacket &&pkt) : pkt_(pkt){};  // TODO: Confirm this needs std::move if call to constructor ALSO has move
  Packet();                               // For optional<> construction

  // Returns a (more) human-readable string of the packet
  virtual std::string to_string() const;

  // Is a response packet expected when this packet is sent.  Defaults to true since
  // most requests receive a response.
  bool is_response_expected() const { return response_expected_; };
  void set_response_expected(bool expect_response) { response_expected_ = expect_response; };

  // Passthrough methods to RawPacket
  RawPacket &raw_packet() { return pkt_; };
  uint8_t get_packet_type() const { return pkt_.get_packet_type(); }
  bool is_checksum_valid() const { return pkt_.is_checksum_valid(); };

  // Returns flags (ONLY APPLICABLE FOR SOME COMMANDS)
  // TODO: Probably combine these a bit?
  uint8_t get_flags() const { return pkt_.get_payload_byte(PLINDEX_FLAGS); }
  uint8_t get_flags_2() const { return pkt_.get_payload_byte(PLINDEX_FLAGS2); }
  // Sets flags (ONLY APPLICABLE FOR SOME COMMANDS)
  void set_flags(uint8_t flag_value);
  // Adds a flag (ONLY APPLICABLE FOR SOME COMMANDS)
  void add_flag(uint8_t flag_to_add);
  // Adds a flag2 (ONLY APPLICABLE FOR SOME COMMANDS)
  void add_flag2(uint8_t flag2_to_add);

  SourceBridge get_source_bridge() const { return pkt_.get_source_bridge(); }
  ControllerAssociation get_controller_association() const { return pkt_.get_controller_association(); }

 protected:
  static const int PLINDEX_FLAGS = 1;
  static const int PLINDEX_FLAGS2 = 2;

  RawPacket pkt_;

 private:
  bool response_expected_ = true;
};

class PacketProcessor {
 public:
  virtual void process_packet(const Packet &packet){};
  virtual void process_packet(const ConnectRequestPacket &packet){};
  virtual void process_packet(const ConnectResponsePacket &packet){};
  virtual void process_packet(const CapabilitiesRequestPacket &packet){};
  virtual void process_packet(const CapabilitiesResponsePacket &packet){};
  virtual void process_packet(const GetRequestPacket &packet){};
  virtual void process_packet(const SettingsGetResponsePacket &packet){};
  virtual void process_packet(const CurrentTempGetResponsePacket &packet){};
  virtual void process_packet(const StatusGetResponsePacket &packet){};
  virtual void process_packet(const RunStateGetResponsePacket &packet){};
  virtual void process_packet(const ErrorStateGetResponsePacket &packet){};
  virtual void process_packet(const Functions1GetResponsePacket &packet){};
  virtual void process_packet(const Functions2GetResponsePacket &packet){};
  virtual void process_packet(const SettingsSetRequestPacket &packet){};
  virtual void process_packet(const RemoteTemperatureSetRequestPacket &packet){};
  virtual void process_packet(const ThermostatSensorStatusPacket &packet){};
  virtual void process_packet(const ThermostatHelloPacket &packet){};
  virtual void process_packet(const ThermostatStateUploadPacket &packet){};
  virtual void process_packet(const ThermostatStateDownloadResponsePacket &packet){};
  virtual void process_packet(const ThermostatAASetRequestPacket &packet){};
  virtual void process_packet(const ThermostatABGetResponsePacket &packet){};
  virtual void process_packet(const SetResponsePacket &packet){};

  virtual void handle_thermostat_state_download_request(const GetRequestPacket &packet){};
  virtual void handle_thermostat_ab_get_request(const GetRequestPacket &packet){};
};

}  // namespace itp_packet
