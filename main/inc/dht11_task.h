#ifndef __DHT11_TASK_H__
#define __DHT11_TASK_H__

#include "dht11.h"

extern struct dht11_info info_dht11;
extern int temp_dht11, hum_dht11, dht11_cnt;

void dht11_task(void *pvParameter);

#endif
