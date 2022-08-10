#ifndef __CJSON_PROCESS_H__
#define __CJSON_PROCESS_H__

#include <time.h>

#include "mqtt.h"

char *cjson_create_dht11(int temp, int humi, time_t now);

#endif
