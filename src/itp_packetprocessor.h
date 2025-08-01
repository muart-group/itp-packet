#pragma once

#include "itp_packets.h"

namespace itp_packet {
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