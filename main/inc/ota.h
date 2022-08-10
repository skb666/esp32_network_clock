#ifndef __OTA_H__
#define __OTA_H__

#include "esp_ota_ops.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

// 配置信息
#define OTA_SERVER_IP "0.0.0.0"
#define OTA_SERVER_PORT "8080"
#define OTA_FILENAME "/downloadFile/esp32_project.bin"

extern EventGroupHandle_t g_ota_event;
extern const int OTA_SUCCESS_BIT;
extern const int OTA_FAILURE_BIT;
extern const int OTA_UPGRADE_BIT;
extern const int OTA_ROLLBACK_BIT;
extern const int OTA_RESET_BIT;
extern const int OTA_UNCHANGED_BIT;

void init_ota(void);

#endif
