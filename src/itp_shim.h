#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdio>

// Shim to decouple framework-specific functions from this library. The dependent application
// will need to implement these functions to allow the library to log and track duration.

namespace itp_packet {

enum class LogLevel : uint8_t { ERROR, WARN, INFO, DEBUG, VERBOSE };

// Must be defined by consuming application to provide logging
void itp_log_write(LogLevel level, const char *tag, const char *message);

// Must be defined by consuming application to provide logging to provide a source of milliseconds for duration
// comparisons
uint32_t itp_millis();

// Formats multi-argument log statements into a single char buffer to be sent to application's log implementation
__attribute__((format(printf, 3, 4))) inline void itp_log_printf(LogLevel level, const char *tag, const char *fmt,
                                                                 ...) {
  char buf[1024];  // Relatively large buffer, but we've got room
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  itp_log_write(level, tag, buf);
}

// For simplicity we'll piggypack on the ESPHOME_LOG_LEVEL so ESPHome configs will translate.
// We need to redefine these here because ESPHOME_LOG_LEVEL isn't set directly to an integer, it's
// set to one of these
#define ESPHOME_LOG_LEVEL_NONE 0
#define ESPHOME_LOG_LEVEL_ERROR 1
#define ESPHOME_LOG_LEVEL_WARN 2
#define ESPHOME_LOG_LEVEL_INFO 3
#define ESPHOME_LOG_LEVEL_CONFIG 4
#define ESPHOME_LOG_LEVEL_DEBUG 5
#define ESPHOME_LOG_LEVEL_VERBOSE 6
#define ESPHOME_LOG_LEVEL_VERY_VERBOSE 7

#ifndef ESPHOME_LOG_LEVEL
#define ESPHOME_LOG_LEVEL 7  // Default to 7 so codepaths followed by IDE
#endif

#if ESPHOME_LOG_LEVEL >= 1
#define ITP_LOGE(tag, fmt, ...) ::itp_packet::itp_log_printf(::itp_packet::LogLevel::ERROR, tag, fmt, ##__VA_ARGS__)
#else
#define ITP_LOGE(tag, fmt, ...)
#endif

#if ESPHOME_LOG_LEVEL >= 2
#define ITP_LOGW(tag, fmt, ...) ::itp_packet::itp_log_printf(::itp_packet::LogLevel::WARN, tag, fmt, ##__VA_ARGS__)
#else
#define ITP_LOGW(tag, fmt, ...)
#endif

#if ESPHOME_LOG_LEVEL >= 3
#define ITP_LOGI(tag, fmt, ...) ::itp_packet::itp_log_printf(::itp_packet::LogLevel::INFO, tag, fmt, ##__VA_ARGS__)
#else
#define ITP_LOGI(tag, fmt, ...)
#endif

#if ESPHOME_LOG_LEVEL >= 5
#define ITP_LOGD(tag, fmt, ...) ::itp_packet::itp_log_printf(::itp_packet::LogLevel::DEBUG, tag, fmt, ##__VA_ARGS__)
#else
#define ITP_LOGD(tag, fmt, ...)
#endif

#if ESPHOME_LOG_LEVEL >= 6
#define ITP_LOGV(tag, fmt, ...) ::itp_packet::itp_log_printf(::itp_packet::LogLevel::VERBOSE, tag, fmt, ##__VA_ARGS__)
#else
#define ITP_LOGV(tag, fmt, ...)
#endif

}  // namespace itp_packet