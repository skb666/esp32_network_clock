#include "ota.h"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "esp_bit_defs.h"
#include "esp_err.h"
#include "freertos/task.h"
#include "my_log.h"

#define TAG "OTA"

// 数据包长度
#define BUFFSIZE 1024
#define TEXT_BUFFSIZE 1024

// OTA数据
static char ota_write_data[BUFFSIZE + 1] = {0};
// 接收数据
static char text[BUFFSIZE + 1] = {0};
// 镜像大小
static int binary_file_length = 0;
// socket句柄
static int socket_id = -1;

// static TaskHandle_t hd_ota_update;
EventGroupHandle_t g_ota_event;
const int OTA_SUCCESS_BIT = BIT0;
const int OTA_FAILURE_BIT = BIT1;
const int OTA_UPGRADE_BIT = BIT2;
const int OTA_ROLLBACK_BIT = BIT3;
const int OTA_RESET_BIT = BIT4;
const int OTA_UNCHANGED_BIT = BIT5;

/*read buffer by byte still delim ,return read bytes counts*/
static int read_until(char *buffer, char delim, int len) {
    int i = 0;
    while (buffer[i] != delim && i < len) {
        ++i;
    }
    return i + 1;
}

/* resolve a packet from http socket
 * return true if packet including \r\n\r\n that means http packet header finished,start to receive packet body
 * otherwise return false
 * */
static bool read_past_http_header(char text[], int total_len, esp_ota_handle_t update_handle) {
    /* i means current position */
    int i = 0, i_read_len = 0;
    while (text[i] != 0 && i < total_len) {
        i_read_len = read_until(&text[i], '\n', total_len);
        // if we resolve \r\n line,we think packet header is finished
        if (i_read_len == 2) {
            int i_write_len = total_len - (i + 2);
            memset(ota_write_data, 0, BUFFSIZE);
            /*copy first http packet body to write buffer*/
            memcpy(ota_write_data, &(text[i + 2]), i_write_len);

            esp_err_t err = esp_ota_write(update_handle, (const void *)ota_write_data, i_write_len);
            if (err != ESP_OK) {
                MY_LOGE(TAG, "Error: esp_ota_write failed! err=0x%x", err);
                return false;
            } else {
                MY_LOGI(TAG, "esp_ota_write header OK");
                binary_file_length += i_write_len;
            }
            return true;
        }
        i += i_read_len;
    }
    return false;
}

static bool connect_to_http_server() {
    MY_LOGI(TAG, "Server IP: %s Server Port:%s", OTA_SERVER_IP, OTA_SERVER_PORT);

    int http_connect_flag = -1;
    struct sockaddr_in sock_info;
    //新建socket
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id == -1) {
        MY_LOGE(TAG, "Create socket failed!");
        return false;
    }

    //设置连接参数
    memset(&sock_info, 0, sizeof(struct sockaddr_in));
    sock_info.sin_family = AF_INET;
    sock_info.sin_addr.s_addr = inet_addr(OTA_SERVER_IP);
    sock_info.sin_port = htons(atoi(OTA_SERVER_PORT));

    //连http服务器
    http_connect_flag = connect(socket_id, (struct sockaddr *)&sock_info, sizeof(sock_info));
    if (http_connect_flag == -1) {
        MY_LOGE(TAG, "Connect to server failed! errno=%d", errno);
        close(socket_id);
        return false;
    } else {
        MY_LOGI(TAG, "Connected to server");
        return true;
    }
    return false;
}

//异常处理，连接http服务器失败等异常
static void __attribute__((noreturn)) task_fatal_error() {
    MY_LOGE(TAG, "Exiting task due to fatal error...");
    xEventGroupSetBits(g_ota_event, OTA_FAILURE_BIT);
    close(socket_id);
    (void)vTaskDelete(NULL);

    while (1) {
        ;
    }
}

// OTA任务
static void ota_update_task(void *pvParameter) {
    esp_err_t err;
    /* update handle : set by esp_ota_begin(), must be freed via esp_ota_end() */
    esp_ota_handle_t update_handle = 0;
    const esp_partition_t *update_partition = NULL;

    MY_LOGI(TAG, "Starting OTA task...");
    //获取当前boot位置
    const esp_partition_t *configured = esp_ota_get_boot_partition();
    //获取当前系统执行的固件所在的Flash分区
    const esp_partition_t *running = esp_ota_get_running_partition();

    if (configured != running) {
        MY_LOGW(TAG, "Configured OTA boot partition at offset 0x%08x, but running from offset 0x%08x", configured->address, running->address);
        MY_LOGW(TAG, "(This can happen if either the OTA boot data or preferred boot image become corrupted somehow.)");
    }
    MY_LOGI(TAG, "Running partition type %d subtype %d (offset 0x%08x)", running->type, running->subtype, running->address);

    //等待wifi连上后进行OTA，项目中可以使升级命令进入OTA
    EventBits_t uxBits = xEventGroupWaitBits(g_ota_event, OTA_UPGRADE_BIT | OTA_ROLLBACK_BIT | OTA_RESET_BIT, false, false, portMAX_DELAY);
    if (uxBits & OTA_UPGRADE_BIT) {
        MY_LOGI(TAG, "Get UPGRADE flag! Start to Connect to Server...");

        //连http服务器
        if (connect_to_http_server()) {
            MY_LOGI(TAG, "Connected to http server");
        } else {
            MY_LOGE(TAG, "Connect to http server failed!");
            task_fatal_error();
        }

        //组http包发送
        const char *GET_FORMAT =
            "GET %s HTTP/1.0\r\n"
            "Host: %s:%s\r\n"
            "User-Agent: esp-idf/4.4 esp32\r\n\r\n";

        char *http_request = NULL;
        int get_len = asprintf(&http_request, GET_FORMAT, OTA_FILENAME, OTA_SERVER_IP, OTA_SERVER_PORT);
        if (get_len < 0) {
            MY_LOGE(TAG, "Failed to allocate memory for GET request buffer");
            task_fatal_error();
        }
        int res = send(socket_id, http_request, get_len, 0);
        free(http_request);
        if (res < 0) {
            MY_LOGE(TAG, "Send GET request to server failed");
            task_fatal_error();
        } else {
            MY_LOGI(TAG, "Send GET request to server succeeded");
        }

        //获取当前系统下一个（紧邻当前使用的OTA_X分区）可用于烧录升级固件的Flash分区
        update_partition = esp_ota_get_next_update_partition(NULL);
        MY_LOGI(TAG, "Writing to partition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);
        assert(update_partition != NULL);
        // OTA写开始
        err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
        if (err != ESP_OK) {
            MY_LOGE(TAG, "esp_ota_begin failed, error=%d", err);
            task_fatal_error();
        }
        MY_LOGI(TAG, "esp_ota_begin succeeded");

        bool resp_body_start = false, flag = true;
        //接收完成
        while (flag) {
            memset(text, 0, TEXT_BUFFSIZE);
            memset(ota_write_data, 0, BUFFSIZE);
            //接收http包
            int buff_len = recv(socket_id, text, TEXT_BUFFSIZE, 0);
            if (buff_len < 0) {  //包异常
                MY_LOGE(TAG, "Error: receive data error! errno=%d", errno);
                task_fatal_error();
            } else if (buff_len > 0 && !resp_body_start) {  //包头
                memcpy(ota_write_data, text, buff_len);
                resp_body_start = read_past_http_header(text, buff_len, update_handle);
            } else if (buff_len > 0 && resp_body_start) {  //数据段包
                memcpy(ota_write_data, text, buff_len);
                //写flash
                err = esp_ota_write(update_handle, (const void *)ota_write_data, buff_len);
                if (err != ESP_OK) {
                    MY_LOGE(TAG, "Error: esp_ota_write failed! err=0x%x", err);
                    task_fatal_error();
                }
                binary_file_length += buff_len;
                MY_LOGD(TAG, "Have written image length %d", binary_file_length);
            } else if (buff_len == 0) {  //结束包
                flag = false;
                MY_LOGI(TAG, "Connection closed, all packets received");
                close(socket_id);
            } else {  //未知错误
                MY_LOGE(TAG, "Unexpected recv result");
            }
        }

        MY_LOGI(TAG, "Total Write binary data length : %d", binary_file_length);
        // OTA写结束
        if (esp_ota_end(update_handle) != ESP_OK) {
            MY_LOGE(TAG, "esp_ota_end failed!");
            task_fatal_error();
        }
        //升级完成更新OTA data区数据，重启时根据OTA data区数据到Flash分区加载执行目标（新）固件
        err = esp_ota_set_boot_partition(update_partition);
        if (err != ESP_OK) {
            MY_LOGE(TAG, "esp_ota_set_boot_partition failed! err=0x%x", err);
            task_fatal_error();
        }
        MY_LOGI(TAG, "Prepare to restart system!");
        xEventGroupSetBits(g_ota_event, OTA_SUCCESS_BIT);
        xEventGroupClearBits(g_ota_event, OTA_UPGRADE_BIT);
        (void)vTaskDelete(NULL);
    } else if (uxBits & OTA_ROLLBACK_BIT) {
        MY_LOGI(TAG, "Get ROLLBACK flag!");
        //获取当前系统下一个（紧邻当前使用的OTA_X分区）可用于烧录升级固件的Flash分区
        esp_partition_t *next_part = esp_ota_get_next_update_partition(NULL);
        esp_partition_t *invalid_part = esp_ota_get_last_invalid_partition();
        assert(next_part != NULL);
        if (running->subtype != 0) {  // 当前使用的分区不是boot
            MY_LOGI(TAG, "Rollback");
            esp_ota_mark_app_invalid_rollback_and_reboot();
        }
        xEventGroupSetBits(g_ota_event, OTA_UNCHANGED_BIT);
        xEventGroupClearBits(g_ota_event, OTA_ROLLBACK_BIT);
        xTaskCreatePinnedToCore(&ota_update_task, "ota_update", 8192, NULL, 5, NULL, 1);
        (void)vTaskDelete(NULL);
    } else if (uxBits & OTA_RESET_BIT) {
        MY_LOGI(TAG, "Get RESET flag! Nothing...");
        xEventGroupSetBits(g_ota_event, OTA_UNCHANGED_BIT);
        xEventGroupClearBits(g_ota_event, OTA_RESET_BIT);
        xTaskCreatePinnedToCore(&ota_update_task, "ota_update", 8192, NULL, 5, NULL, 1);
        (void)vTaskDelete(NULL);
    }
}

void init_ota(void) {
    g_ota_event = xEventGroupCreate();
    //开启http任务，内部等mqtt发送的update指令后进行http交互
    xTaskCreatePinnedToCore(&ota_update_task, "ota_update", 8192, NULL, 5, NULL, 1);
}
