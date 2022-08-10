#ifndef __MQTT_H__
#define __MQTT_H__

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "mqtt_client.h"

/* host & no user: 0x00,
 * uri & no user: 0x01,
 * host & user: 0x10,
 * uri & user: 0x11 */
#define MY_MQTT_MODE 0x11
#define MY_MQTT_PORT 1883
#define MY_MQTT_CID "esp32_project"
#if (MY_MQTT_MODE & 0x01)
#define MY_MQTT_URI "mqtt://0.0.0.0"
#else
#define MY_MQTT_HOST ""
#endif
#if (MY_MQTT_MODE & 0x10)
#define MY_MQTT_USERNAME "user"
#define MY_MQTT_PASSWORD "user_password"
#endif

extern esp_mqtt_client_handle_t g_mqtt_client;
extern EventGroupHandle_t g_mqtt_event;
extern const int MQTT_CONNECTED;
extern const int MQTT_DISCONNECTED;
extern const int MQTT_GET_ORDER;
extern const int REQ_ACTIVITY_BIT;
extern const int REQ_DATANOW_BIT;
extern const int REQ_PARTITION_BIT;
extern const int REQ_REBOOT_BIT;

void init_mqtt(void);

#endif
