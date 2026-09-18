#pragma once

#include "itp_packets.h"

// TODO: Not sure which side some of the enhanced mhk2 packets should land on-- leave them on both for now
namespace itp_packet {
// Implemented by any class that wants to receive packet data from the ITP heat pump or thermostat.
// This interface is limited to data/state containing packets (e.g. no SetRequest, ConnectResponse, etc.)
class ITPPacketReceiver {
 public:
  virtual void receive_packet(const Packet &packet){};

  // Heatpump packets
  virtual void receive_packet(const CapabilitiesResponsePacket &packet){};
  virtual void receive_packet(const CurrentTempGetResponsePacket &packet){};
  virtual void receive_packet(const ErrorStateGetResponsePacket &packet){};
  virtual void receive_packet(const Functions1GetResponsePacket &packet){};
  virtual void receive_packet(const Functions2GetResponsePacket &packet){};
  virtual void receive_packet(const RunStateGetResponsePacket &packet){};
  virtual void receive_packet(const SettingsGetResponsePacket &packet){};
  virtual void receive_packet(const StatusGetResponsePacket &packet){};
  virtual void receive_packet(const ZoneGetResponsePacket &packet){};

  // Thermostat packets
  virtual void receive_packet(const RemoteTemperatureSetRequestPacket &packet){};
  virtual void receive_packet(const ThermostatAASetRequestPacket &packet){};
  virtual void receive_packet(const ThermostatABGetResponsePacket &packet){};
  virtual void receive_packet(const ThermostatHelloPacket &packet){};
  virtual void receive_packet(const ThermostatSensorStatusPacket &packet){};
  virtual void receive_packet(const ThermostatStateDownloadResponsePacket &packet){};
  virtual void receive_packet(const ThermostatStateUploadPacket &packet){};
  virtual void receive_packet(const SettingsSetRequestPacket &packet){};
};
}