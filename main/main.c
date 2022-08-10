#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cjson_process.h"
#include "dht11_task.h"
#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "gui_task.h"
#include "led.h"
#include "mqtt.h"
#include "my_log.h"
#include "ota.h"
#include "sntp.h"
#include "wifi_sc.h"
#include "ws2812.h"

#define TAG "APP_MAIN"

static void update_screen(void *pvParameters);
static void wifi_event_process(void *pvParameters);
static void ota_event_process(void *pvParameters);
static void mqtt_event_process(void *pvParameters);

int g_sc_running = 0;

// 主函数
void app_main() {
    MY_LOGI(TAG, "APP is start!~");
    // 初始化led
    led_init();
    // 初始化ws2812
    ws2812_init();

    init_wifi();
    init_sntp();
    init_ota();
    init_mqtt();

    xTaskCreatePinnedToCore(wifi_event_process, "wifi_event", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(ota_event_process, "ota_event", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(mqtt_event_process, "mqtt_event", 4096, NULL, 1, NULL, 0);
    // 如果要使用任务创建图形，则需要创建固定任务,否则可能会出现诸如内存损坏等问题
    xTaskCreatePinnedToCore(gui_task, "gui", 4096 * 2, NULL, 0, NULL, 1);
    xTaskCreatePinnedToCore(dht11_task, "dht11", 4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(update_screen, "screen", 4096, NULL, 5, NULL, 1);

    MY_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
}

static void update_screen(void *pvParameters) {
    static int time_cnt = 0;
    time_t now;
    struct tm timeinfo;
    char *week_day[] = {"Sun.", "Mon.", "Tue.", "Wed.", "Thu.", "Fri.", "Sat."};
    TickType_t xLastWakeTime;
    const TickType_t xDelay1s = pdMS_TO_TICKS(1000);
    xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil(&xLastWakeTime, xDelay1s);
        time_cnt = (time_cnt + 1) % (60 * 15);
        // update 'now' variable with current time
        time(&now);
        // Set timezone to China Standard Time
        setenv("TZ", "CST-8", 1);
        tzset();
        localtime_r(&now, &timeinfo);
        lv_label_set_text_fmt(ui.screen_date, "%04d.%02d.%02d %s", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, week_day[timeinfo.tm_wday]);
        lv_label_set_text_fmt(ui.screen_time, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        if (time_cnt == 8) {
            MY_LOGI(TAG, "Tabview append new date");
            temp_dht11 /= dht11_cnt;
            hum_dht11 /= dht11_cnt;
            dht11_cnt = 1;
            lv_label_set_text_fmt(ui.screen_temp_label, "temperature:  %d °C", temp_dht11);
            lv_chart_set_next(ui.screen_temp_line, ui.screen_temp_line_0, temp_dht11);
            lv_label_set_text_fmt(ui.screen_hum_label, "humidity: %d%%", hum_dht11);
            lv_gauge_set_value(ui.screen_hum_graph, 0, hum_dht11);
            char *buf = cjson_create_dht11(temp_dht11, hum_dht11, now);
            int msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/data", buf, 0, 0, 0);
            free(buf);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        }
    }
}

static void wifi_event_process(void *pvParameters) {
    EventBits_t uxBits;
    while (1) {
        uxBits = xEventGroupWaitBits(g_wifi_event, WIFI_CONNECTED | WIFI_DISCONNECTED | WIFI_SC_START | WIFI_SC_FINISH, false, false, portMAX_DELAY);
        if (uxBits & WIFI_SC_START) {
            MY_LOGI(TAG, "WIFI_SC_START");
            ws2812_setColor(WS2812_ALL);
            g_sc_running = 1;
            xEventGroupClearBits(g_wifi_event, WIFI_SC_START);
        } else if (uxBits & WIFI_SC_FINISH) {
            MY_LOGI(TAG, "WIFI_SC_FINISH");
            g_sc_running = 0;
            xEventGroupClearBits(g_wifi_event, WIFI_SC_FINISH);
        }
        if (uxBits & WIFI_CONNECTED) {
            MY_LOGI(TAG, "WIFI_CONNECTED");
            led_on();
            lv_label_set_text_fmt(ui.screen_ip_info, "IP: " IPSTR "/%d | GW: " IPSTR, IP2STR(&ip_info.ip), convert_mask(ip_info.netmask.addr), IP2STR(&ip_info.gw));
            xEventGroupClearBits(g_wifi_event, WIFI_CONNECTED);
        } else if (uxBits & WIFI_DISCONNECTED) {
            MY_LOGI(TAG, "WIFI_DISCONNECTED");
            led_off();
            xEventGroupClearBits(g_wifi_event, WIFI_DISCONNECTED);
        }
    }
}

static void ota_event_process(void *pvParameters) {
    EventBits_t uxBits;
    int msg_id;
    while (1) {
        uxBits = xEventGroupWaitBits(g_ota_event, OTA_SUCCESS_BIT | OTA_FAILURE_BIT | OTA_UNCHANGED_BIT, true, false, portMAX_DELAY);
        if (g_sc_running) continue;
        if (uxBits & OTA_SUCCESS_BIT) {
            MY_LOGI(TAG, "OTA_SUCCESS_BIT");
            ws2812_setColor(WS2812_RED_GREEN);
            msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/event", "SUCCESS", 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            xEventGroupClearBits(g_ota_event, OTA_SUCCESS_BIT);
        } else if (uxBits & OTA_FAILURE_BIT) {
            MY_LOGI(TAG, "OTA_FAILURE_BIT");
            ws2812_setColor(WS2812_BLUE_RED);
            msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/event", "FAILURE", 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            xEventGroupClearBits(g_ota_event, OTA_FAILURE_BIT);
        } else if (uxBits & OTA_UNCHANGED_BIT) {
            MY_LOGI(TAG, "OTA_UNCHANGED_BIT");
            ws2812_setColor(WS2812_BLUE_GREEN);
            msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/event", "UNCHANGED", 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            xEventGroupClearBits(g_ota_event, OTA_UNCHANGED_BIT);
        }
    }
}

static void mqtt_event_process(void *pvParameters) {
    EventBits_t uxBits;
    time_t now;
    int t_temp, t_humi, msg_id;
    while (1) {
        uxBits = xEventGroupWaitBits(g_mqtt_event, MQTT_CONNECTED | MQTT_DISCONNECTED | MQTT_GET_ORDER | REQ_ACTIVITY_BIT | REQ_DATANOW_BIT | REQ_PARTITION_BIT | REQ_REBOOT_BIT, false, false, portMAX_DELAY);
        if (!g_sc_running && (uxBits & MQTT_CONNECTED)) {
            MY_LOGI(TAG, "MQTT_CONNECTED");
            ws2812_setColor(WS2812_GREEN);
            xEventGroupClearBits(g_mqtt_event, MQTT_CONNECTED);
        }
        if (!g_sc_running && (uxBits & MQTT_DISCONNECTED)) {
            MY_LOGI(TAG, "MQTT_DISCONNECTED");
            ws2812_setColor(WS2812_RED);
            xEventGroupClearBits(g_mqtt_event, MQTT_DISCONNECTED);
        }
        if (!g_sc_running && (uxBits & MQTT_GET_ORDER)) {
            MY_LOGI(TAG, "MQTT_GET_ORDER");
            ws2812_setColor(WS2812_BLUE);
            xEventGroupClearBits(g_mqtt_event, MQTT_GET_ORDER);
        }
        if (uxBits & REQ_REBOOT_BIT) {
            MY_LOGI(TAG, "REQ_REBOOT_BIT");
            msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/event", "REBOOT", 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            xEventGroupClearBits(g_mqtt_event, REQ_REBOOT_BIT);
            // 重启
            esp_restart();
        }
        if (uxBits & REQ_ACTIVITY_BIT) {
            MY_LOGI(TAG, "REQ_ACTIVITY_BIT");
            msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/event", "ACTIVITY", 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            xEventGroupClearBits(g_mqtt_event, REQ_ACTIVITY_BIT);
        }
        if (uxBits & REQ_DATANOW_BIT) {
            MY_LOGI(TAG, "REQ_TIMENOW_BIT");
            time(&now);
            t_temp = temp_dht11 / dht11_cnt;
            t_humi = hum_dht11 / dht11_cnt;
            msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/event", "DATANOW", 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            char *buf = cjson_create_dht11(t_temp, t_humi, now);
            msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/response", buf, 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            free(buf);
            xEventGroupClearBits(g_mqtt_event, REQ_DATANOW_BIT);
        }
        if (uxBits & REQ_PARTITION_BIT) {
            MY_LOGI(TAG, "REQ_PARTITION_BIT");
            msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/event", "PARTITION", 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            //获取当前系统执行的固件所在的Flash分区
            const esp_partition_t *running = esp_ota_get_running_partition();
            switch (running->address) {
                case 0x10000:
                    msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/response", "Factory", 0, 0, 0);
                    break;
                case 0x160000:
                    msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/response", "OTA_0", 0, 0, 0);
                    break;
                case 0x2b0000:
                    msg_id = esp_mqtt_client_publish(g_mqtt_client, "/esp32_project/response", "OTA_1", 0, 0, 0);
                    break;
            }
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            xEventGroupClearBits(g_mqtt_event, REQ_PARTITION_BIT);
        }
    }
}
