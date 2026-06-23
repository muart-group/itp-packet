#pragma once

#include "itp_packet.h"

namespace itp_packet {
class ConnectRequestPacket : public Packet {
 public:
  using Packet::Packet;
  static ConnectRequestPacket &instance() {
    static ConnectRequestPacket instance;
    instance.set_sequence(next_seq_++);
    return instance;
  }

  std::string to_string() const override;
  static bool validate_type(const RawPacket &pkt) {
    return pkt.get_packet_type() == static_cast<uint8_t>(PacketType::CONNECT_REQUEST);
  }

 private:
  ConnectRequestPacket() : Packet(RawPacket(PacketType::CONNECT_REQUEST, 2)) {
    pkt_.set_payload_byte(0, 0xca);
    pkt_.set_payload_byte(1, 0x01);
  }
};

class ConnectResponsePacket : public Packet {
 public:
  using Packet::Packet;
  std::string to_string() const override;
  static bool validate_type(const RawPacket &pkt) {
    return pkt.get_packet_type() == static_cast<uint8_t>(PacketType::CONNECT_RESPONSE);
  }
};

}  // namespace itp_packet