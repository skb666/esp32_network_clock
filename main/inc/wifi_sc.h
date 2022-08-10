#ifndef __WIFI_SC_H__
#define __WIFI_SC_H__

#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

// 宏定义WiFi更新标识码、WiFi名称和密码
#define MY_WIFI_UPDATE 1  // 对数值进行修改表示更新NVS的WiFi名称和密码
#define MY_WIFI_SSID "wifi_ssid"
#define MY_WIFI_PASSWD "wifi_password"

extern esp_netif_ip_info_t ip_info;
extern EventGroupHandle_t g_wifi_event;
extern const int WIFI_CONNECTED;
extern const int WIFI_DISCONNECTED;
extern const int WIFI_SC_START;
extern const int WIFI_SC_FINISH;

int convert_mask(uint32_t mask);

void init_wifi(void);

#endif
