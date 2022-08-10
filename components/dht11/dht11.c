#include "dht11.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DHT11_PIN 26

static gpio_num_t dht_gpio;
static int64_t last_read_time = -2000000;
static struct dht11_info last_read;

static int _waitOrTimeout(uint16_t microSeconds, int level) {
    int micros_ticks = 0;
    while (gpio_get_level(dht_gpio) == level) {
        if (micros_ticks++ > microSeconds)
            return DHT11_TIMEOUT_ERROR;
        ets_delay_us(1);
    }
    return micros_ticks;
}

static int _checkCRC(uint8_t data[]) {
    if (data[4] == (data[0] + data[1] + data[2] + data[3])) {
        return DHT11_OK;
    } else {
        return DHT11_CRC_ERROR;
    }
}

static void _sendStartSignal() {
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_gpio, 0);
    ets_delay_us(20 * 1000);
    gpio_set_level(dht_gpio, 1);
    ets_delay_us(40);
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);
}

static int _checkResponse() {
    /* Wait for next step ~80us*/
    if (_waitOrTimeout(80, 0) == DHT11_TIMEOUT_ERROR)
        return DHT11_TIMEOUT_ERROR;

    /* Wait for next step ~80us*/
    if (_waitOrTimeout(80, 1) == DHT11_TIMEOUT_ERROR)
        return DHT11_TIMEOUT_ERROR;

    return DHT11_OK;
}

static struct dht11_info _timeoutError() {
    struct dht11_info timeoutError = {DHT11_TIMEOUT_ERROR, -1, -1};
    return timeoutError;
}

static struct dht11_info _crcError() {
    struct dht11_info crcError = {DHT11_CRC_ERROR, -1, -1};
    return crcError;
}

void dht11_init() {
    /* Wait 3 seconds to make the device pass its initial unstable status */
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    dht_gpio = DHT11_PIN;
}

struct dht11_info dht11_read() {
    /* Tried to sense too son since last read (dht11 needs ~2 seconds to make a new read) */
    if (esp_timer_get_time() - 2000000 < last_read_time) {
        return last_read;
    }

    last_read_time = esp_timer_get_time();

    uint8_t data[5] = {0, 0, 0, 0, 0};

    _sendStartSignal();

    if (_checkResponse() == DHT11_TIMEOUT_ERROR)
        return last_read = _timeoutError();

    /* Read response */
    for (int i = 0; i < 40; i++) {
        /* Initial data */
        if (_waitOrTimeout(50, 0) == DHT11_TIMEOUT_ERROR)
            return last_read = _timeoutError();

        if (_waitOrTimeout(70, 1) > 28) {
            /* Bit received was a 1 */
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    if (_checkCRC(data) != DHT11_CRC_ERROR) {
        last_read.status = DHT11_OK;
        last_read.temperature = data[2];
        last_read.humidity = data[0];
        return last_read;
    } else {
        return last_read = _crcError();
    }
}
