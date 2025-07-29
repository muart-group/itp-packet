#include "connect.h"

namespace itp_packet {

std::string ConnectRequestPacket::to_string() const { return ("Connect Request: " + Packet::to_string()); }
std::string ConnectResponsePacket::to_string() const { return ("Connect Response: " + Packet::to_string()); }

}  // namespace itp_packet