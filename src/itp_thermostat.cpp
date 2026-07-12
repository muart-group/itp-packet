#include "itp_thermostat.h"

namespace itp_packet {

Thermostat::Thermostat(ITPByteProvider *byte_provider, Heatpump *connected_heatpump, ITPSystemState *sys_state)
    : ITPPacketReader(byte_provider, "Thermostat"), connected_heatpump_{*connected_heatpump}, sys_state_{*sys_state} {}

void Thermostat::loop() {
  if (in_flight_request_.is_running()) {
    // If there is already a request being processed, don't do anything else and just wait for it to return.
    // TODO: This is where we should check to see if the thermostat has sent another packet and cancel the inflight one
  } else {
    if (std::optional<RawPacket> rp = check_for_packet()) {
      in_flight_request_ = handle_thermostat_request(rp.value());
    }
  }
}

// TODO: This might be a better place to check for cache before sending off to heatpump.
Task Thermostat::handle_thermostat_request(RawPacket &raw_request_packet) {
  // If temperature correction is on, adjust temperatures
  if (mhk_fahrenheit_correction_) {
    raw_request_packet = adjust_mhk_temperature(raw_request_packet);
  }

  switch (static_cast<PacketType>(raw_request_packet.get_packet_type())) {
    case PacketType::CONNECT_REQUEST:
      return send_to_heatpump<ConnectRequestPacket, ConnectResponsePacket>(raw_request_packet);
    case PacketType::IDENTIFY_REQUEST:
      return send_to_heatpump<IdentifyCDRequestPacket, IdentifyCDResponsePacket>(raw_request_packet);
    case PacketType::GET_REQUEST:
      switch (static_cast<GetCommand>(raw_request_packet.get_command())) {
        case GetCommand::SETTINGS:
          return send_to_heatpump<GetRequestPacket, SettingsGetResponsePacket>(raw_request_packet);
        case GetCommand::CURRENT_TEMP:
          return send_to_heatpump<GetRequestPacket, CurrentTempGetResponsePacket>(raw_request_packet);
        case GetCommand::ERROR_INFO:
          return send_to_heatpump<GetRequestPacket, ErrorStateGetResponsePacket>(raw_request_packet);
        case GetCommand::RUN_STATE:
          return send_to_heatpump<GetRequestPacket, RunStateGetResponsePacket>(raw_request_packet);
        case GetCommand::STATUS:
          return send_to_heatpump<GetRequestPacket, StatusGetResponsePacket>(raw_request_packet);
        case GetCommand::FUNCTIONS_1:
          return send_to_heatpump<GetRequestPacket, Functions1GetResponsePacket>(raw_request_packet);
        case GetCommand::FUNCTIONS_2:
          return send_to_heatpump<GetRequestPacket, Functions2GetResponsePacket>(raw_request_packet);
        case GetCommand::ZONE_STATE:
          return send_to_heatpump<GetRequestPacket, ZoneGetResponsePacket>(raw_request_packet);
        case GetCommand::THERMOSTAT_STATE_DOWNLOAD:
          if (enhanced_mhk_) {
            return send_immediately(get_state_download_response());
          } else {
            return send_to_heatpump<GetRequestPacket, ThermostatStateDownloadResponsePacket>(raw_request_packet);
          }
        case GetCommand::THERMOSTAT_GET_AB:
          if (enhanced_mhk_) {
            return send_immediately(ThermostatABGetResponsePacket());
          } else {
            return send_to_heatpump<GetRequestPacket, ThermostatStateDownloadResponsePacket>(raw_request_packet);
          }
        default:
          // Unknown GET_REQUEST goes to default
          goto unknown_packet;
      }
    case PacketType::SET_REQUEST:
      switch (static_cast<SetCommand>(raw_request_packet.get_command())) {
        case SetCommand::REMOTE_TEMPERATURE:
          if (intercept_remote_temp_) {
            // If we're intercepting, cache in incoming packet (to notify MITP for temperature recording)
            sys_state_.cache_thermostat_packet(RemoteTemperatureSetRequestPacket(std::move(raw_request_packet)), true);
            // And immediately return a response without contacting heatpump
            return send_immediately(SetResponsePacket());
          } else {
            return send_to_heatpump<RemoteTemperatureSetRequestPacket, SetResponsePacket>(raw_request_packet);
          }
        case SetCommand::SETTINGS:
          // Send to cache for ThermostatCommandReceivedSensor
          sys_state_.cache_thermostat_packet(SettingsSetRequestPacket(std::move(raw_request_packet)), true);
          return send_to_heatpump<SettingsSetRequestPacket, SetResponsePacket>(raw_request_packet);
        case SetCommand::THERMOSTAT_SENSOR_STATUS:
          if (enhanced_mhk_) {
            // No processing to be done here, it's just forwarded to sensors
            sys_state_.cache_thermostat_packet(ThermostatSensorStatusPacket(std::move(raw_request_packet)), true);
            return send_immediately(SetResponsePacket());
          } else {
            return send_to_heatpump<ThermostatSensorStatusPacket, SetResponsePacket>(raw_request_packet);
          }
        case SetCommand::THERMOSTAT_HELLO:
          // TODO: Log this info?
          if (enhanced_mhk_) {
            sys_state_.cache_thermostat_packet(ThermostatHelloPacket(std::move(raw_request_packet)), true);
            return send_immediately(SetResponsePacket());
          } else {
            return send_to_heatpump<ThermostatHelloPacket, SetResponsePacket>(raw_request_packet);
          }

        case SetCommand::THERMOSTAT_STATE_UPLOAD:
          if (enhanced_mhk_) {
            handle_state_upload(raw_request_packet);
            return send_immediately(SetResponsePacket());
          } else {
            return send_to_heatpump<ThermostatStateUploadPacket, SetResponsePacket>(raw_request_packet);
          }
        case SetCommand::ZONE_STATE:
          return send_to_heatpump<ZoneSetRequestPacket, SetResponsePacket>(raw_request_packet);
        case SetCommand::THERMOSTAT_SET_AA:
          if (enhanced_mhk_) {
            return send_immediately(SetResponsePacket());
          } else {
            return send_to_heatpump<ThermostatAASetRequestPacket, SetResponsePacket>(raw_request_packet);
          }
        default:
          // Unknown SET_REQUEST goes to default
          goto unknown_packet;
      }

    default:
    unknown_packet:
      ITP_LOGI(THERMOSTAT_TAG, "Unexpected thermostat packet type %02X/%02X", raw_request_packet.get_packet_type(),
               raw_request_packet.get_command());
      return send_to_heatpump<Packet, UnknownPacket>(raw_request_packet);
  };
}

Task Thermostat::send_immediately(Packet packet) {
  write_raw_packet_(packet.raw_packet());
  co_return;
}

void Thermostat::write_raw_packet_(const RawPacket &packet_to_send) const {
  byte_provider_.write_array(packet_to_send.get_bytes(), packet_to_send.get_length());
}

RawPacket Thermostat::adjust_mhk_temperature(RawPacket &raw_pkt) {
  // Set Remote
  if (raw_pkt.get_packet_type() == static_cast<uint8_t>(PacketType::SET_REQUEST) &&
      raw_pkt.get_command() == static_cast<uint8_t>(SetCommand::REMOTE_TEMPERATURE)) {
    RemoteTemperatureSetRequestPacket temp_pkt = RemoteTemperatureSetRequestPacket(std::move(raw_pkt));
    ITP_LOGV(THERMOSTAT_TAG, "Adjusting MHK temp from %f", temp_pkt.get_remote_temperature());
    temp_pkt.set_remote_temperature(mhk_temp_to_actual(temp_pkt.get_remote_temperature()));
    ITP_LOGV(THERMOSTAT_TAG, "Adjusted MHK temp to %f", temp_pkt.get_remote_temperature());
    return temp_pkt.raw_packet();
  }
  // Set Target
  else if (raw_pkt.get_packet_type() == static_cast<uint8_t>(PacketType::SET_REQUEST) &&
           raw_pkt.get_command() == static_cast<uint8_t>(SetCommand::SETTINGS)) {
    SettingsSetRequestPacket temp_pkt = SettingsSetRequestPacket(std::move(raw_pkt));
    ITP_LOGV(THERMOSTAT_TAG, "Adjusting MHK temp from %f", temp_pkt.get_target_temp());
    temp_pkt.set_target_temperature(mhk_temp_to_actual(temp_pkt.get_target_temp()));
    ITP_LOGV(THERMOSTAT_TAG, "Adjusted MHK temp to %f", temp_pkt.get_target_temp());
    return temp_pkt.raw_packet();
  }
  // Get Current
  else if (raw_pkt.get_packet_type() == static_cast<uint8_t>(PacketType::GET_RESPONSE) &&
           raw_pkt.get_command() == static_cast<uint8_t>(GetCommand::CURRENT_TEMP)) {
    CurrentTempGetResponsePacket temp_pkt = CurrentTempGetResponsePacket(std::move(raw_pkt));
    ITP_LOGV(THERMOSTAT_TAG, "Adjusting MHK temp from %f", temp_pkt.get_current_temp());
    temp_pkt.set_current_temperature(mhk_temp_from_actual(temp_pkt.get_current_temp()));
    ITP_LOGV(THERMOSTAT_TAG, "Adjusted MHK temp to %f", temp_pkt.get_current_temp());
    return temp_pkt.raw_packet();
  }
  // Get Target
  else if (raw_pkt.get_packet_type() == static_cast<uint8_t>(PacketType::GET_RESPONSE) &&
           raw_pkt.get_command() == static_cast<uint8_t>(GetCommand::SETTINGS)) {
    SettingsGetResponsePacket temp_pkt = SettingsGetResponsePacket(std::move(raw_pkt));
    ITP_LOGV(THERMOSTAT_TAG, "Adjusting MHK temp from %f", temp_pkt.get_target_temp());
    temp_pkt.set_target_temperature(mhk_temp_from_actual(temp_pkt.get_target_temp()));
    ITP_LOGV(THERMOSTAT_TAG, "Adjusted MHK temp to %f", temp_pkt.get_target_temp());
    return temp_pkt.raw_packet();
  } else {
    return raw_pkt;
  }
}

void Thermostat::handle_state_upload(RawPacket &raw_pkt) {
  auto packet = ThermostatStateUploadPacket(std::move(raw_pkt));
  if (packet.get_flags() & 0x04) {
    auto_mode_ = packet.get_auto_mode();
  }
  if (packet.get_flags() & 0x08) {
    heat_setpoint_ =
        mhk_fahrenheit_correction_ ? mhk_temp_to_actual(packet.get_heat_setpoint()) : packet.get_heat_setpoint();
    if (mhk_fahrenheit_correction_) {
      ITP_LOGD(THERMOSTAT_TAG, "handle_state_upload Fahrenheit Conversion %f -> %f", packet.get_heat_setpoint(),
               mhk_temp_to_actual(packet.get_heat_setpoint()));
    }
  }
  if (packet.get_flags() & 0x10) {
    cooldry_setpoint_ =
        mhk_fahrenheit_correction_ ? mhk_temp_to_actual(packet.get_cool_setpoint()) : packet.get_cool_setpoint();
    if (mhk_fahrenheit_correction_) {
      ITP_LOGD(THERMOSTAT_TAG, "handle_state_upload Fahrenheit Conversion %f -> %f", packet.get_cool_setpoint(),
               mhk_temp_to_actual(packet.get_cool_setpoint()));
    }
  }

  sys_state_.cache_thermostat_packet(packet);  // Don't always notify to reduce repeated timestamp processing
}

ThermostatStateDownloadResponsePacket Thermostat::get_state_download_response() {
  ThermostatStateDownloadResponsePacket response = ThermostatStateDownloadResponsePacket();

  response = response.set_timestamp(get_timestruct_());

  response.set_auto_mode(auto_mode_);
  response.set_cool_setpoint(mhk_fahrenheit_correction_ ? mhk_temp_from_actual(cooldry_setpoint_) : cooldry_setpoint_);
  if (mhk_fahrenheit_correction_) {
    ITP_LOGD(THERMOSTAT_TAG, "get_state_download_response Fahrenheit Conversion %f -> %f", cooldry_setpoint_,
             mhk_temp_from_actual(cooldry_setpoint_));
  }
  response.set_heat_setpoint(mhk_fahrenheit_correction_ ? mhk_temp_from_actual(heat_setpoint_) : heat_setpoint_);
  if (mhk_fahrenheit_correction_) {
    ITP_LOGD(THERMOSTAT_TAG, "get_state_download_response Fahrenheit Conversion %f -> %f", heat_setpoint_,
             mhk_temp_from_actual(heat_setpoint_));
  }

  ITP_LOGD(THERMOSTAT_TAG, "Sending %s", response.to_string().c_str());

  return response;
}

}  // namespace itp_packet