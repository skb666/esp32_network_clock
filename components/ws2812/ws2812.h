#ifndef __WS2812_DRIVER_H__
#define __WS2812_DRIVER_H__

#include <stdint.h>

typedef union {
    struct __attribute__((packed)) {
        uint8_t r, g, b;
    };
    uint32_t num;
} rgbVal;

extern void ws2812_init(void);
extern void ws2812_setColors(unsigned int length, rgbVal *array);
extern void ws2812_setColor(rgbVal color);

inline rgbVal makeRGBVal(uint8_t r, uint8_t g, uint8_t b) {
    rgbVal v;
    v.r = r;
    v.g = g;
    v.b = b;
    return v;
}

#define WS2812_NONE makeRGBVal(0, 0, 0)
#define WS2812_ALL makeRGBVal(1, 1, 1)
#define WS2812_GREEN makeRGBVal(0, 1, 0)
#define WS2812_RED makeRGBVal(1, 0, 0)
#define WS2812_RED_GREEN makeRGBVal(1, 1, 0)
#define WS2812_BLUE makeRGBVal(0, 0, 1)
#define WS2812_BLUE_GREEN makeRGBVal(0, 1, 1)
#define WS2812_BLUE_RED makeRGBVal(1, 0, 1)

#endif /* WS2812_DRIVER_H */
