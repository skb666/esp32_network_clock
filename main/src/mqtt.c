#include "mqtt.h"

#include "esp_bit_defs.h"
#include "my_log.h"
#include "ota.h"

#define TAG "MQTT"

#define TOPIC_STR(STR) (strncmp(event->topic, STR, event->topic_len) == 0)
#define DATA_STR(STR) (strncmp(event->data, STR, event->data_len) == 0)

esp_mqtt_client_handle_t g_mqtt_client;
EventGroupHandle_t g_mqtt_event;
const int MQTT_CONNECTED = BIT0;
const int MQTT_DISCONNECTED = BIT1;
const int MQTT_GET_ORDER = BIT2;
const int REQ_ACTIVITY_BIT = BIT3;
const int REQ_DATANOW_BIT = BIT4;
const int REQ_PARTITION_BIT = BIT5;
const int REQ_REBOOT_BIT = BIT6;

static void log_error_if_nonzero(const char *message, int error_code) {
    if (error_code != 0) {
        MY_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    // MY_LOGI(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            MY_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xEventGroupSetBits(g_mqtt_event, MQTT_CONNECTED);
            msg_id = esp_mqtt_client_publish(client, "/esp32_project/event", "CONNECTED", 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            msg_id = esp_mqtt_client_subscribe(client, "/esp32_project/control", 0);
            MY_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            MY_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            xEventGroupSetBits(g_mqtt_event, MQTT_DISCONNECTED);
            break;
        case MQTT_EVENT_SUBSCRIBED:
            MY_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/esp32_project/event", "SUBSCRIBED", 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            MY_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/esp32_project/event", "UNSUBSCRIBED", 0, 0, 0);
            MY_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            MY_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            MY_LOGI(TAG, "MQTT_EVENT_DATA");
            MY_LOGC(TAG, LOG_COLOR_PURPLE "TOPIC: %.*s\n" LOG_COLOR_RESET, event->topic_len, event->topic);
            MY_LOGC(TAG, LOG_COLOR_PURPLE "DATA: %.*s\n" LOG_COLOR_RESET, event->data_len, event->data);
            if (TOPIC_STR("/esp32_project/control")) {
                if (DATA_STR("UPGRADE")) {
                    MY_LOGI(TAG, "SET UPGRADE BIT");
                    xEventGroupSetBits(g_ota_event, OTA_UPGRADE_BIT);
                    xEventGroupSetBits(g_mqtt_event, MQTT_GET_ORDER);
                } else if (DATA_STR("ROLLBACK")) {
                    MY_LOGI(TAG, "SET ROLLBACK BIT");
                    xEventGroupSetBits(g_ota_event, OTA_ROLLBACK_BIT);
                    xEventGroupSetBits(g_mqtt_event, MQTT_GET_ORDER);
                } else if (DATA_STR("RESET")) {
                    MY_LOGI(TAG, "SET RESET BIT");
                    xEventGroupSetBits(g_ota_event, OTA_RESET_BIT);
                    xEventGroupSetBits(g_mqtt_event, MQTT_GET_ORDER);
                } else if (DATA_STR("REBOOT")) {
                    MY_LOGI(TAG, "SET REBOOT BIT");
                    xEventGroupSetBits(g_mqtt_event, REQ_REBOOT_BIT | MQTT_GET_ORDER);
                } else if (DATA_STR("ACTIVITY")) {
                    MY_LOGI(TAG, "SET ACTIVITY BIT");
                    xEventGroupSetBits(g_mqtt_event, REQ_ACTIVITY_BIT);
                } else if (DATA_STR("DATANOW")) {
                    MY_LOGI(TAG, "SET DATANOW BIT");
                    xEventGroupSetBits(g_mqtt_event, REQ_DATANOW_BIT);
                } else if (DATA_STR("PARTITION")) {
                    MY_LOGI(TAG, "SET PARTITION BIT");
                    xEventGroupSetBits(g_mqtt_event, REQ_PARTITION_BIT);
                }
            }
            break;
        case MQTT_EVENT_ERROR:
            MY_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
                MY_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            MY_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void init_mqtt(void) {
    g_mqtt_event = xEventGroupCreate();
    // 初始化mqtt配置
    esp_mqtt_client_config_t mqtt_run_cfg = {
        .client_id = MY_MQTT_CID,
#ifdef MY_MQTT_URI
        .uri = MY_MQTT_URI,
#else
        .host = MY_MQTT_HOST,
#endif
        .port = MY_MQTT_PORT,
#ifdef MY_MQTT_USERNAME
        .username = MY_MQTT_USERNAME,
        .password = MY_MQTT_PASSWORD,
#endif
    };
    g_mqtt_client = esp_mqtt_client_init(&mqtt_run_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(g_mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(g_mqtt_client);
}
