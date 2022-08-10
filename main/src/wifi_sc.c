#include "wifi_sc.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_bit_defs.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_smartconfig.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "my_log.h"
#include "nvs_flash.h"

#define TAG "WIFI_SC"

esp_netif_ip_info_t ip_info;

// CIDR转斜杠
int convert_mask(uint32_t mask) {
    int count = 0;
    while (mask & 1) {
        mask >>= 1;
        count++;
    }
    return count;
}

// wifi链接成功事件
EventGroupHandle_t g_wifi_event;
const int WIFI_CONNECTED = BIT0;
const int WIFI_DISCONNECTED = BIT1;
const int WIFI_SC_START = BIT2;
const int WIFI_SC_FINISH = BIT3;
static EventGroupHandle_t wifi_event;
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static TaskHandle_t hd_smartconfig;
static nvs_handle hd_nvs;
static uint8_t ssid[33];
static uint8_t password[65];
static uint8_t rvd_data[65];

static void smartconfig_task(void *parm) {
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_V2));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
    MY_LOGI(TAG, "SmartConfig start...");
    xEventGroupSetBits(g_wifi_event, WIFI_SC_START);
    while (1) {
        uxBits = xEventGroupWaitBits(wifi_event, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY);
        if (uxBits & CONNECTED_BIT) {
            MY_LOGI(TAG, "SmartConfig success");
            // 将得到的WiFi名称和密码存入NVS
            ESP_ERROR_CHECK(nvs_open("WiFi_Config", NVS_READWRITE, &hd_nvs));
            ESP_ERROR_CHECK(nvs_set_str(hd_nvs, "wifi_ssid", (const char *)ssid));
            ESP_ERROR_CHECK(nvs_set_str(hd_nvs, "wifi_passwd", (const char *)password));
            ESP_ERROR_CHECK(nvs_commit(hd_nvs));  // 提交
            nvs_close(hd_nvs);                    // 关闭
            MY_LOGI(TAG, "SmartConfig save WiFi_Config to NVS");
        }
        if (uxBits & ESPTOUCH_DONE_BIT) {
            MY_LOGI(TAG, "SmartConfig over");
            xEventGroupSetBits(g_wifi_event, WIFI_SC_FINISH);
            esp_smartconfig_stop();
            vTaskDelete(hd_smartconfig);
        }
    }
}

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    wifi_config_t wifi_config;
    static int retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        size_t len;
        ESP_ERROR_CHECK(nvs_open("WiFi_Config", NVS_READWRITE, &hd_nvs));
        len = sizeof(ssid);  // 从NVS中获取ssid
        ESP_ERROR_CHECK(nvs_get_str(hd_nvs, "wifi_ssid", (char *)ssid, &len));
        len = sizeof(password);  // 从NVS中获取password
        ESP_ERROR_CHECK(nvs_get_str(hd_nvs, "wifi_passwd", (char *)password, &len));
        ESP_ERROR_CHECK(nvs_commit(hd_nvs));  // 提交
        nvs_close(hd_nvs);                    // 关闭
        // 设置wifi参数
        bzero(&wifi_config, sizeof(wifi_config_t));  // 将结构体数据清零
        memcpy(wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
        // 尝试连接wifi
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
        // 清除WiFi连接成功标志位
        xEventGroupClearBits(wifi_event, CONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        xEventGroupSetBits(g_wifi_event, WIFI_DISCONNECTED);
        esp_wifi_connect();
        retry_num++;
        MY_LOGW(TAG, "Reconnecting to AP... %d", retry_num);
        xEventGroupClearBits(wifi_event, CONNECTED_BIT);
        if (retry_num >= 10) {
            xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, &hd_smartconfig);
            retry_num = 0;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        MY_LOGI(TAG, "Got IP: " IPSTR "/%d , GW: " IPSTR, IP2STR(&event->ip_info.ip), convert_mask(event->ip_info.netmask.addr), IP2STR(&event->ip_info.gw));
        ip_info = event->ip_info;
        retry_num = 0;
        xEventGroupSetBits(g_wifi_event, WIFI_CONNECTED);
        xEventGroupSetBits(wifi_event, CONNECTED_BIT);
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        MY_LOGI(TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        MY_LOGI(TAG, "Found channel");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        MY_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        MY_LOGC(TAG, LOG_COLOR_PURPLE "SSID: %s\n" LOG_COLOR_RESET, ssid);
        MY_LOGC(TAG, LOG_COLOR_PURPLE "PASSWORD: %s\n" LOG_COLOR_RESET, password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK(esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
            MY_LOGC(TAG, LOG_COLOR_PURPLE "RVD_DATA: %.*s\n" LOG_COLOR_RESET, sizeof(rvd_data), rvd_data);
        }

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(wifi_event, ESPTOUCH_DONE_BIT);
    }
}

void init_wifi(void) {
    // 初始化非易失性存储库 (NVS)
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS分区被截断，需要删除,然后重新初始化NVS
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    // 检测更新默认wifi配置
    ESP_ERROR_CHECK(nvs_open("WiFi_Config", NVS_READWRITE, &hd_nvs));
    uint32_t wifi_update = 0;
    nvs_get_u32(hd_nvs, "wifi_update", &wifi_update);
    if (MY_WIFI_UPDATE == wifi_update) {
        MY_LOGI(TAG, "WiFi_Config needn't to update.");
    } else {
        MY_LOGI(TAG, "WiFi_Config update now...");
        ESP_ERROR_CHECK(nvs_set_str(hd_nvs, "wifi_ssid", MY_WIFI_SSID));
        ESP_ERROR_CHECK(nvs_set_str(hd_nvs, "wifi_passwd", MY_WIFI_PASSWD));
        ESP_ERROR_CHECK(nvs_set_u32(hd_nvs, "wifi_update", MY_WIFI_UPDATE));
        MY_LOGI(TAG, "WiFi_Config update ok. \n");
    }
    ESP_ERROR_CHECK(nvs_commit(hd_nvs)); /* 提交 */
    nvs_close(hd_nvs);                   /* 关闭 */

    // 初始化底层TCP/IP堆栈。在应用程序启动时，应该调用此函数一次
    ESP_ERROR_CHECK(esp_netif_init());
    // 事件组
    wifi_event = xEventGroupCreate();
    g_wifi_event = xEventGroupCreate();
    // 创建默认事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    //  创建一个默认的WIFI-STA网络接口，如果初始化错误，此API将中止。
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);
    // wifi设置:默认设置，等待sc配置
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // 根据cfg参数初始化wifi连接所需要的资源
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    // 注册wifi事件
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    // 启动wifi
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}
