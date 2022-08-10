#include "dht11_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "my_log.h"

#define TAG "DHT11_TASK"

struct dht11_info info_dht11;

int temp_dht11, hum_dht11, dht11_cnt;

void dht11_task(void *pvParameters) {
    dht11_init();
    while (1) {
        info_dht11 = dht11_read();
        if (info_dht11.status == DHT11_OK) {
            temp_dht11 += info_dht11.temperature;
            hum_dht11 += info_dht11.humidity;
            dht11_cnt++;
            MY_LOGI(TAG, "temp -> %i °C  |  hum -> %i%%", info_dht11.temperature, info_dht11.humidity);
        } else {
            MY_LOGI(TAG, "DHT11 data got error!");
        }
        vTaskDelay(60000 / portTICK_RATE_MS);
    }
}
