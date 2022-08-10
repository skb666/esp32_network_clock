#include "cjson_process.h"

#include <string.h>

#include "cJson.h"
#include "my_log.h"

#define TAG "CJSON_PROCESS"

char *cjson_create_dht11(int temp, int humi, time_t now) {
    cJSON *root = cJSON_CreateObject();
    cJSON *temp_p = cJSON_CreateNumber(temp);
    cJSON *humi_p = cJSON_CreateNumber(humi);
    cJSON *time_p = cJSON_CreateNumber(now);
    cJSON_AddItemToObject(root, "temp", temp_p);
    cJSON_AddItemToObject(root, "humi", humi_p);
    cJSON_AddItemToObject(root, "time", time_p);
    MY_LOGI(TAG, "cJson malloc");
    return cJSON_Print(root);
}
