file(GLOB_RECURSE SOURCES *.c)
idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS inc
                    REQUIRES lvgl_esp32_drivers lvgl_touch lvgl_tft lvgl led ws2812 dht11 mqtt json app_update
                    PRIV_REQUIRES nvs_flash)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
