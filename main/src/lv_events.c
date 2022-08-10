#include "lv_events.h"

#include "mqtt.h"
#include "my_log.h"
#include "ota.h"

#define TAG "LV_EVENTS"

static void screen_btn1event_handler(lv_obj_t *obj, lv_event_t event) {
    switch (event) {
        case LV_EVENT_CLICKED: {
            MY_LOGI(TAG, "UPGRADE");
            xEventGroupSetBits(g_ota_event, OTA_UPGRADE_BIT);
            xEventGroupSetBits(g_mqtt_event, MQTT_GET_ORDER);
        } break;
        default:
            break;
    }
}

static void screen_btn2event_handler(lv_obj_t *obj, lv_event_t event) {
    switch (event) {
        case LV_EVENT_CLICKED: {
            MY_LOGI(TAG, "ROLLBACK");
            xEventGroupSetBits(g_ota_event, OTA_ROLLBACK_BIT);
            xEventGroupSetBits(g_mqtt_event, MQTT_GET_ORDER);
        } break;
        default:
            break;
    }
}

void lv_events_init(lv_ui *ui) {
    lv_obj_set_event_cb(ui->screen_btn1, screen_btn1event_handler);
    lv_obj_set_event_cb(ui->screen_btn2, screen_btn2event_handler);
}
