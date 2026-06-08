#pragma once

#include "itp_packets.h"

// TODO: Not sure which side some of the enhanced mhk2 packets should land on-- leave them on both for now
// TODO: We should probably only list the useful-data-containing packets here (i.e. those that will be cached)
// since I don't think ESPHome needsto know about e.g. SettingsSetRequestPacket being sent anywhere
namespace itp_packet {
// Implemented by classes that want to receieve packets sent *from* the heatpump. Generally these are
// *ResponsePackets, but won't include any of the requests sent by the thermostat.
class HeatpumpPacketReceiver {
 public:
  virtual void receive_packet(const Packet &packet){};
  virtual void receive_packet(const CapabilitiesResponsePacket &packet){};
  virtual void receive_packet(const CurrentTempGetResponsePacket &packet){};
  virtual void receive_packet(const ErrorStateGetResponsePacket &packet){};
  virtual void receive_packet(const Functions1GetResponsePacket &packet){};
  virtual void receive_packet(const Functions2GetResponsePacket &packet){};
  virtual void receive_packet(const RunStateGetResponsePacket &packet){};
  virtual void receive_packet(const SettingsGetResponsePacket &packet){};
  virtual void receive_packet(const StatusGetResponsePacket &packet){};
  virtual void receive_packet(const ZoneGetResponsePacket &packet){};
};

// Implemented by classes that want to receieve packets sent *from* the thermostat. This includes
// a lot of status requests and set requests that will probably be less useful, but also e.g.
// ThermostatSensorStatusPacket with info about humidity and battery state
class ThermostatPacketReceiver {
 public:
  virtual void receive_packet(const Packet &packet){};
  virtual void receive_packet(const RemoteTemperatureSetRequestPacket &packet){};
  virtual void receive_packet(const ThermostatAASetRequestPacket &packet){};
  virtual void receive_packet(const ThermostatABGetResponsePacket &packet){};
  virtual void receive_packet(const ThermostatHelloPacket &packet){};
  virtual void receive_packet(const ThermostatSensorStatusPacket &packet){};
  virtual void receive_packet(const ThermostatStateDownloadResponsePacket &packet){};
  virtual void receive_packet(const ThermostatStateUploadPacket &packet){};

  // virtual void handle_thermostat_state_download_request(const GetRequestPacket &packet){};
  // virtual void handle_thermostat_ab_get_request(const GetRequestPacket &packet){};
};
}  // namespace itp_packet