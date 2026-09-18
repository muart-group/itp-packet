#include "itp_packet.h"

namespace itp_packet {

// Creates an empty packet
Packet::Packet() {
  // TODO: Is this okay?
}

std::string Packet::to_string() const {
  // Based on `format_hex_pretty` from ESPHome
  if (pkt_.get_length() < PACKET_HEADER_SIZE)
    return "";
  std::string out_string;

  out_string += CONSOLE_COLOR_GRAY;
  out_string += '(' + std::to_string(this->get_sequence()) + ')';

  out_string += CONSOLE_COLOR_CYAN;
  out_string += '[';

  for (size_t i = 0; i < PACKET_HEADER_SIZE; i++) {
    if (i == 1) {
      out_string += CONSOLE_COLOR_CYAN_BOLD;
    }
    out_string += ITPUtils::format_hex_pretty_char((pkt_.get_bytes()[i] & 0xF0) >> 4);
    out_string += ITPUtils::format_hex_pretty_char(pkt_.get_bytes()[i] & 0x0F);
    if (i < PACKET_HEADER_SIZE - 1) {
      out_string += '.';
    }
    if (i == 1) {
      out_string += CONSOLE_COLOR_CYAN;
    }
  }
  // Header close-bracket
  out_string += ']';
  out_string += CONSOLE_COLOR_WHITE;  // White

  // Payload
  for (size_t i = PACKET_HEADER_SIZE; i < pkt_.get_length() - 1; i++) {
    out_string += ITPUtils::format_hex_pretty_char((pkt_.get_bytes()[i] & 0xF0) >> 4);
    out_string += ITPUtils::format_hex_pretty_char(pkt_.get_bytes()[i] & 0x0F);
    if (i < pkt_.get_length() - 2) {
      out_string += '.';
    }
  }

  // Space
  out_string += ' ';
  out_string += CONSOLE_COLOR_GREEN;  // Green

  // Checksum
  out_string += ITPUtils::format_hex_pretty_char((pkt_.get_bytes()[pkt_.get_length() - 1] & 0xF0) >> 4);
  out_string += ITPUtils::format_hex_pretty_char(pkt_.get_bytes()[pkt_.get_length() - 1] & 0x0F);

  out_string += CONSOLE_COLOR_NONE;  // Reset

  return out_string;
}

void Packet::set_flags(const uint8_t flag_value) { pkt_.set_payload_byte(PLINDEX_FLAGS, flag_value); }

// Adds a flag (ONLY APPLICABLE FOR SOME COMMANDS)
void Packet::add_flag(const uint8_t flag_to_add) {
  pkt_.set_payload_byte(PLINDEX_FLAGS, pkt_.get_payload_byte(PLINDEX_FLAGS) | flag_to_add);
}
// Adds a flag2 (ONLY APPLICABLE FOR SOME COMMANDS)
void Packet::add_flag2(const uint8_t flag2_to_add) {
  pkt_.set_payload_byte(PLINDEX_FLAGS2, pkt_.get_payload_byte(PLINDEX_FLAGS2) | flag2_to_add);
}

}  // namespace itp_packet
