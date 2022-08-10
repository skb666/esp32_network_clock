#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lv_gui.h"
#include "lvgl/lvgl.h"     // LVGL头文件
#include "lvgl_helpers.h"  // 助手 硬件驱动相关
#include "my_log.h"
#include "ota.h"

#define TAG "GUI_TASK"
#define LV_TICK_PERIOD_MS 10
SemaphoreHandle_t xGuiSemaphore;  // 创建一个GUI信号量

// LVGL 时钟任务
static void lv_tick_task(void *arg) {
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void tabview_loop(void *arg) {
    int i = 0;
    int tab_count = lv_tabview_get_tab_count(ui.screen_tabview);
    MY_LOGD(TAG, "tab_count: %d", tab_count);
    while (1) {
        vTaskDelay(20000 / portTICK_PERIOD_MS);
        i = lv_tabview_get_tab_act(ui.screen_tabview);
        MY_LOGD(TAG, "tab_index: %d", i);
        lv_tabview_set_tab_act(ui.screen_tabview, (i + 1) % tab_count, LV_ANIM_ON);
    }
}

static void create_application(void) {
    lv_gui();
    //获取当前系统执行的固件所在的Flash分区
    const esp_partition_t *running = esp_ota_get_running_partition();
    MY_LOGI(TAG, "Running from offset 0x%08x", running->address);
    switch (running->address) {
        case 0x10000:
            lv_label_set_text(ui.screen_tabview_Time_label, "Factory");
            break;
        case 0x160000:
            lv_label_set_text(ui.screen_tabview_Time_label, "OTA_0");
            break;
        case 0x2b0000:
            lv_label_set_text(ui.screen_tabview_Time_label, "OTA_1");
            break;
    }
    xTaskCreatePinnedToCore(tabview_loop, "tabview", 4096, NULL, 0, NULL, 1);
}

void gui_task(void *pvParameter) {
    (void)pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();  // 创建GUI信号量
    lv_init();                                // 初始化LittlevGL
    lvgl_driver_init();                       // 初始化液晶SPI驱动 触摸芯片SPI/IIC驱动

    // 初始化缓存
    static lv_color_t buf1[DISP_BUF_SIZE];
    static lv_color_t buf2[DISP_BUF_SIZE];
    static lv_disp_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    // 添加并注册触摸驱动
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    // 添加并注册触摸驱动
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);

    // 定期处理GUI回调
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui",
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    MY_LOGI(TAG, "Create Application");
    // 创建GUI
    create_application();

    while (1) {
        vTaskDelay(1);
        // 尝试锁定信号量，如果成功，请调用lvgl的东西
        if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);  // 释放信号量
        }
    }
    vTaskDelete(NULL);  // 删除任务
}
