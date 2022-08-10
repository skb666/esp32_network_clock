#include "driver/gpio.h"

#define LED_GPIO 4

void led_init(void) {
    gpio_pad_select_gpio(LED_GPIO);                  // 选择要操作的GPIO
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);  // 设置GPIO为推挽输出模式
    gpio_set_level(LED_GPIO, 1);
}

void led_on(void) {
    gpio_set_level(LED_GPIO, 0);
}

void led_off(void) {
    gpio_set_level(LED_GPIO, 1);
}
