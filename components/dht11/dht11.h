#ifndef __DHT11_H__
#define __DHT11_H__

enum dht11_status {
    DHT11_CRC_ERROR = -2,
    DHT11_TIMEOUT_ERROR,
    DHT11_OK
};

struct dht11_info {
    int status;
    int temperature;
    int humidity;
};

void dht11_init();
struct dht11_info dht11_read();

#endif
