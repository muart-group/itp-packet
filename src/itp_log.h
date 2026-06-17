#pragma once

// Shim to decouple itp-packet from the whole ESP-IDF framewprk. If you're using this within an
// ESPHome or ESP-IDF project, it should transparently use the correct ESP_LOG* macros. You can define
// your own by defining your own ITP_LOG.

#ifndef ITP_LOG

#if __has_include(<esp_log.h>)
#include <esp_log.h>
#define ITP_LOGI(tag, fmt, ...) ESP_LOGI(tag, fmt, ##__VA_ARGS__)
#define ITP_LOGW(tag, fmt, ...) ESP_LOGW(tag, fmt, ##__VA_ARGS__)
#define ITP_LOGE(tag, fmt, ...) ESP_LOGE(tag, fmt, ##__VA_ARGS__)
#define ITP_LOGD(tag, fmt, ...) ESP_LOGD(tag, fmt, ##__VA_ARGS__)
#define ITP_LOGV(tag, fmt, ...) ESP_LOGV(tag, fmt, ##__VA_ARGS__)
#else
#include <cstdio>
#define ITP_LOGI(tag, fmt, ...) printf("[I][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ITP_LOGW(tag, fmt, ...) printf("[W][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ITP_LOGE(tag, fmt, ...) printf("[E][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ITP_LOGD(tag, fmt, ...) printf("[D][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define ITP_LOGV(tag, fmt, ...) printf("[V][%s] " fmt "\n", tag, ##__VA_ARGS__)
#endif

#endif  // ITP_LOGI

// For ESP-IDF projects, hal.h provides millis(), otherwise you'll need to provide your own
#if __has_include(<hal.h>)
#include <hal.h>
#define itp_millis() esphome::millis()
#else
#define itp_millis() 0;
#endif