#include <time.h>

#include "esp_sntp.h"
#include "my_log.h"

#define TAG "SNTP"

static void time_sync_notification_cb(struct timeval *tv) {
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];
    MY_LOGI(TAG, "Notification of a time synchronization event");
    // update 'now' variable with current time
    time(&now);
    // Set timezone to China Standard Time
    setenv("TZ", "CST-8", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    MY_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);
}

void init_sntp(void) {
    MY_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "b-b.icu");
    sntp_setservername(1, "ntp.aliyun.com");
    sntp_setservername(2, "cn.pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}

