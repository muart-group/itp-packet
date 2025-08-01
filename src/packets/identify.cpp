#include "identify.h"

namespace itp_packet {
std::string IdentifyCDResponsePacket::to_string() const { return "Identify CD Response: " + Packet::to_string(); }
}  // namespace itp_packet