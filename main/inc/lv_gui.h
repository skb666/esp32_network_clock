#ifndef __LV_GUI_H__
#define __LV_GUI_H__

#include "lvgl/lvgl.h"  // LVGL头文件

typedef struct {
    lv_obj_t *screen;
    lv_obj_t *screen_tabview;
    lv_obj_t *screen_tabview_Time;
    lv_obj_t *screen_tabview_Time_label;
    lv_obj_t *screen_time;
    lv_obj_t *screen_date;
    lv_obj_t *screen_ip_info;
    lv_obj_t *screen_tabview_Temp;
    lv_obj_t *screen_temp_label;
    lv_obj_t *screen_temp_line;
    lv_obj_t *screen_tabview_Humidity;
    lv_obj_t *screen_hum_label;
    lv_obj_t *screen_hum_graph;
    lv_obj_t *screen_btn1;
    lv_obj_t *screen_btn1_label;
    lv_obj_t *screen_btn2;
    lv_obj_t *screen_btn2_label;
    lv_chart_series_t *screen_temp_line_0;
} lv_ui;

extern lv_ui ui;

void lv_gui();

#endif
