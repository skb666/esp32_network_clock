#include "lv_gui.h"

#include "lv_events.h"
#include "my_log.h"

#define TAG "LV_GUI"

lv_ui ui;

void lv_gui() {
    ui.screen = lv_disp_get_scr_act(NULL);                   // 返回屏幕的指针
    ui.screen_tabview = lv_tabview_create(ui.screen, NULL);  // 在屏幕上创建一个TabView控件

    // Write style LV_OBJ_PART_MAIN for screen
    static lv_style_t style_screen_main;
    lv_style_reset(&style_screen_main);

    // Write style state: LV_STATE_DEFAULT for style_screen_main
    lv_style_set_bg_color(&style_screen_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_opa(&style_screen_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui.screen, LV_OBJ_PART_MAIN, &style_screen_main);
    // lv_anim_path_t lv_anim_path_screen_tabview = {.cb = lv_anim_path_linear};

    // Time
    ui.screen_tabview_Time = lv_tabview_add_tab(ui.screen_tabview, "Time");
    ui.screen_tabview_Time_label = lv_label_create(ui.screen_tabview_Time, NULL);
    lv_label_set_text(ui.screen_tabview_Time_label, "Factory");

    // Temp
    ui.screen_tabview_Temp = lv_tabview_add_tab(ui.screen_tabview, "Temp");

    // Humi
    ui.screen_tabview_Humidity = lv_tabview_add_tab(ui.screen_tabview, "Humi");

    lv_tabview_set_anim_time(ui.screen_tabview, 300);

    // Write style LV_TABVIEW_PART_BG for screen_tabview
    static lv_style_t style_screen_tabview_bg;
    lv_style_reset(&style_screen_tabview_bg);

    // Write style state: LV_STATE_DEFAULT for style_screen_tabview_bg
    lv_style_set_bg_color(&style_screen_tabview_bg, LV_STATE_DEFAULT, lv_color_make(0xd1, 0xf0, 0xff));
    lv_style_set_bg_grad_color(&style_screen_tabview_bg, LV_STATE_DEFAULT, lv_color_make(0xd1, 0xf0, 0xff));
    lv_style_set_bg_grad_dir(&style_screen_tabview_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_tabview_bg, LV_STATE_DEFAULT, 200);
    lv_style_set_border_color(&style_screen_tabview_bg, LV_STATE_DEFAULT, lv_color_make(0xf5, 0xf5, 0xf5));
    lv_style_set_border_width(&style_screen_tabview_bg, LV_STATE_DEFAULT, 1);
    lv_style_set_text_color(&style_screen_tabview_bg, LV_STATE_DEFAULT, lv_color_make(0x5a, 0xa3, 0x18));
    lv_style_set_text_font(&style_screen_tabview_bg, LV_STATE_DEFAULT, lv_theme_get_font_normal());
    lv_style_set_text_letter_space(&style_screen_tabview_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_text_line_space(&style_screen_tabview_bg, LV_STATE_DEFAULT, 16);
    lv_obj_add_style(ui.screen_tabview, LV_TABVIEW_PART_BG, &style_screen_tabview_bg);

    // Write style LV_TABVIEW_PART_INDIC for screen_tabview
    static lv_style_t style_screen_tabview_indic;
    lv_style_reset(&style_screen_tabview_indic);

    // Write style state: LV_STATE_DEFAULT for style_screen_tabview_indic
    lv_style_set_bg_color(&style_screen_tabview_indic, LV_STATE_DEFAULT, lv_color_make(0x1f, 0x4a, 0xe5));
    lv_style_set_bg_grad_color(&style_screen_tabview_indic, LV_STATE_DEFAULT, lv_color_make(0x1f, 0x4a, 0xe5));
    lv_style_set_bg_grad_dir(&style_screen_tabview_indic, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_tabview_indic, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui.screen_tabview, LV_TABVIEW_PART_INDIC, &style_screen_tabview_indic);

    // Write style LV_TABVIEW_PART_TAB_BG for screen_tabview
    static lv_style_t style_screen_tabview_tab_bg;
    lv_style_reset(&style_screen_tabview_tab_bg);

    // Write style state: LV_STATE_DEFAULT for style_screen_tabview_tab_bg
    lv_style_set_bg_color(&style_screen_tabview_tab_bg, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_screen_tabview_tab_bg, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_screen_tabview_tab_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_tabview_tab_bg, LV_STATE_DEFAULT, 255);
    lv_style_set_border_color(&style_screen_tabview_tab_bg, LV_STATE_DEFAULT, lv_color_make(0xfa, 0xfa, 0xfa));
    lv_style_set_border_width(&style_screen_tabview_tab_bg, LV_STATE_DEFAULT, 1);
    lv_style_set_pad_left(&style_screen_tabview_tab_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_screen_tabview_tab_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_screen_tabview_tab_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_screen_tabview_tab_bg, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui.screen_tabview, LV_TABVIEW_PART_TAB_BG, &style_screen_tabview_tab_bg);

    // Write style LV_TABVIEW_PART_TAB_BTN for screen_tabview
    static lv_style_t style_screen_tabview_tab_btn;
    lv_style_reset(&style_screen_tabview_tab_btn);

    // Write style state: LV_STATE_DEFAULT for style_screen_tabview_tab_btn
    lv_style_set_text_color(&style_screen_tabview_tab_btn, LV_STATE_DEFAULT, lv_color_make(0x4d, 0x4d, 0x4d));
    lv_style_set_text_letter_space(&style_screen_tabview_tab_btn, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui.screen_tabview, LV_TABVIEW_PART_TAB_BTN, &style_screen_tabview_tab_btn);
    lv_obj_set_pos(ui.screen_tabview, 0, 0);
    lv_obj_set_size(ui.screen_tabview, 320, 240);

    // Write codes screen_time
    ui.screen_time = lv_label_create(ui.screen_tabview_Time, NULL);
    lv_label_set_text(ui.screen_time, "00:00:00");
    lv_label_set_long_mode(ui.screen_time, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui.screen_time, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for screen_time
    static lv_style_t style_screen_time_main;
    lv_style_reset(&style_screen_time_main);

    // Write style state: LV_STATE_DEFAULT for style_screen_time_main
    lv_style_set_radius(&style_screen_time_main, LV_STATE_DEFAULT, 15);
    lv_style_set_bg_color(&style_screen_time_main, LV_STATE_DEFAULT, lv_color_make(0x6f, 0x82, 0x8b));
    lv_style_set_bg_grad_color(&style_screen_time_main, LV_STATE_DEFAULT, lv_color_make(0x6f, 0x82, 0x8b));
    lv_style_set_bg_grad_dir(&style_screen_time_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_time_main, LV_STATE_DEFAULT, 100);
    lv_style_set_text_color(&style_screen_time_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_text_font(&style_screen_time_main, LV_STATE_DEFAULT, &lv_font_montserrat_36);
    lv_style_set_text_letter_space(&style_screen_time_main, LV_STATE_DEFAULT, 1);
    lv_style_set_pad_left(&style_screen_time_main, LV_STATE_DEFAULT, 10);
    lv_style_set_pad_right(&style_screen_time_main, LV_STATE_DEFAULT, 10);
    lv_style_set_pad_top(&style_screen_time_main, LV_STATE_DEFAULT, 15);
    lv_style_set_pad_bottom(&style_screen_time_main, LV_STATE_DEFAULT, 15);
    lv_obj_add_style(ui.screen_time, LV_LABEL_PART_MAIN, &style_screen_time_main);
    lv_obj_set_pos(ui.screen_time, 20, 85);
    lv_obj_set_size(ui.screen_time, 280, 0);

    // Write codes screen_date
    ui.screen_date = lv_label_create(ui.screen_tabview_Time, NULL);
    lv_label_set_text(ui.screen_date, "2022.05.08 Sun.");
    lv_label_set_long_mode(ui.screen_date, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui.screen_date, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for screen_date
    static lv_style_t style_screen_date_main;
    lv_style_reset(&style_screen_date_main);

    // Write style state: LV_STATE_DEFAULT for style_screen_date_main
    lv_style_set_radius(&style_screen_date_main, LV_STATE_DEFAULT, 15);
    lv_style_set_bg_color(&style_screen_date_main, LV_STATE_DEFAULT, lv_color_make(0x6f, 0x82, 0x8b));
    lv_style_set_bg_grad_color(&style_screen_date_main, LV_STATE_DEFAULT, lv_color_make(0x6f, 0x82, 0x8b));
    lv_style_set_bg_grad_dir(&style_screen_date_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_date_main, LV_STATE_DEFAULT, 100);
    lv_style_set_text_color(&style_screen_date_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_text_font(&style_screen_date_main, LV_STATE_DEFAULT, &lv_font_montserrat_24);
    lv_style_set_text_letter_space(&style_screen_date_main, LV_STATE_DEFAULT, 1);
    lv_style_set_pad_left(&style_screen_date_main, LV_STATE_DEFAULT, 10);
    lv_style_set_pad_right(&style_screen_date_main, LV_STATE_DEFAULT, 10);
    lv_style_set_pad_top(&style_screen_date_main, LV_STATE_DEFAULT, 15);
    lv_style_set_pad_bottom(&style_screen_date_main, LV_STATE_DEFAULT, 15);
    lv_obj_add_style(ui.screen_date, LV_LABEL_PART_MAIN, &style_screen_date_main);
    lv_obj_set_pos(ui.screen_date, 20, 20);
    lv_obj_set_size(ui.screen_date, 280, 0);

    // Write codes screen_ip_info
    ui.screen_ip_info = lv_label_create(ui.screen_tabview_Time, NULL);
    lv_label_set_text(ui.screen_ip_info, "IP: 192.168.1.2/24 | GW: 192.168.1.1");
    lv_label_set_long_mode(ui.screen_ip_info, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui.screen_ip_info, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for screen_ip_info
    static lv_style_t style_screen_ip_info_main;
    lv_style_reset(&style_screen_ip_info_main);

    // Write style state: LV_STATE_DEFAULT for style_screen_ip_info_main
    lv_style_set_radius(&style_screen_ip_info_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_screen_ip_info_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_screen_ip_info_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_screen_ip_info_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_ip_info_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_screen_ip_info_main, LV_STATE_DEFAULT, lv_color_make(0x8d, 0xb1, 0xc3));
    lv_style_set_text_font(&style_screen_ip_info_main, LV_STATE_DEFAULT, &lv_font_montserrat_12);
    lv_style_set_text_letter_space(&style_screen_ip_info_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_left(&style_screen_ip_info_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_screen_ip_info_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_screen_ip_info_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_screen_ip_info_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui.screen_ip_info, LV_LABEL_PART_MAIN, &style_screen_ip_info_main);
    lv_obj_set_pos(ui.screen_ip_info, 20, 162);
    lv_obj_set_size(ui.screen_ip_info, 280, 0);

    // Write codes screen_temp_label
    ui.screen_temp_label = lv_label_create(ui.screen_tabview_Temp, NULL);
    lv_label_set_text(ui.screen_temp_label, "temperature:  27 °C");
    lv_label_set_long_mode(ui.screen_temp_label, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui.screen_temp_label, LV_LABEL_ALIGN_LEFT);

    // Write style LV_LABEL_PART_MAIN for screen_temp_label
    static lv_style_t style_screen_temp_label_main;
    lv_style_reset(&style_screen_temp_label_main);

    // Write style state: LV_STATE_DEFAULT for style_screen_temp_label_main
    lv_style_set_radius(&style_screen_temp_label_main, LV_STATE_DEFAULT, 5);
    lv_style_set_bg_color(&style_screen_temp_label_main, LV_STATE_DEFAULT, lv_color_make(0x6f, 0x82, 0x8b));
    lv_style_set_bg_grad_color(&style_screen_temp_label_main, LV_STATE_DEFAULT, lv_color_make(0x6f, 0x82, 0x8b));
    lv_style_set_bg_grad_dir(&style_screen_temp_label_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_temp_label_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_screen_temp_label_main, LV_STATE_DEFAULT, lv_color_make(0x5a, 0xa3, 0xf2));
    lv_style_set_text_font(&style_screen_temp_label_main, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_style_set_text_letter_space(&style_screen_temp_label_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_left(&style_screen_temp_label_main, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_right(&style_screen_temp_label_main, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_top(&style_screen_temp_label_main, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_bottom(&style_screen_temp_label_main, LV_STATE_DEFAULT, 5);
    lv_obj_add_style(ui.screen_temp_label, LV_LABEL_PART_MAIN, &style_screen_temp_label_main);
    lv_obj_set_pos(ui.screen_temp_label, 15, 11);
    lv_obj_set_size(ui.screen_temp_label, 290, 0);

    // Write codes screen_temp_line
    ui.screen_temp_line = lv_chart_create(ui.screen_tabview_Temp, NULL);

    // Write style LV_CHART_PART_BG for screen_temp_line
    static lv_style_t style_screen_temp_line_bg;
    lv_style_reset(&style_screen_temp_line_bg);

    // Write style state: LV_STATE_DEFAULT for style_screen_temp_line_bg
    lv_style_set_bg_color(&style_screen_temp_line_bg, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_screen_temp_line_bg, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_screen_temp_line_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_temp_line_bg, LV_STATE_DEFAULT, 100);
    lv_style_set_pad_left(&style_screen_temp_line_bg, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_right(&style_screen_temp_line_bg, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_top(&style_screen_temp_line_bg, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_bottom(&style_screen_temp_line_bg, LV_STATE_DEFAULT, 5);
    lv_obj_add_style(ui.screen_temp_line, LV_CHART_PART_BG, &style_screen_temp_line_bg);

    // Write style LV_CHART_PART_SERIES_BG for screen_temp_line
    static lv_style_t style_screen_temp_line_series_bg;
    lv_style_reset(&style_screen_temp_line_series_bg);

    // Write style state: LV_STATE_DEFAULT for style_screen_temp_line_series_bg
    lv_style_set_line_color(&style_screen_temp_line_series_bg, LV_STATE_DEFAULT, lv_color_make(0xe8, 0xe8, 0xe8));
    lv_style_set_line_width(&style_screen_temp_line_series_bg, LV_STATE_DEFAULT, 1);
    lv_style_set_line_opa(&style_screen_temp_line_series_bg, LV_STATE_DEFAULT, 150);
    lv_obj_add_style(ui.screen_temp_line, LV_CHART_PART_SERIES_BG, &style_screen_temp_line_series_bg);
    lv_obj_set_pos(ui.screen_temp_line, 15, 43);
    lv_obj_set_size(ui.screen_temp_line, 290, 135);
    lv_chart_set_type(ui.screen_temp_line, LV_CHART_TYPE_LINE);
    lv_chart_set_range(ui.screen_temp_line, 0, 55);
    lv_chart_set_div_line_count(ui.screen_temp_line, 5, 10);
    lv_chart_set_point_count(ui.screen_temp_line, 10);
    ui.screen_temp_line_0 = lv_chart_add_series(ui.screen_temp_line, lv_color_make(0xf0, 0xb4, 0x60));

    // Write codes screen_tabview_Humidity

    // Write codes screen_hum_label
    ui.screen_hum_label = lv_label_create(ui.screen_tabview_Humidity, NULL);
    lv_label_set_text(ui.screen_hum_label, "humidity: 87%");
    lv_label_set_long_mode(ui.screen_hum_label, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui.screen_hum_label, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for screen_hum_label
    static lv_style_t style_screen_hum_label_main;
    lv_style_reset(&style_screen_hum_label_main);

    // Write style state: LV_STATE_DEFAULT for style_screen_hum_label_main
    lv_style_set_radius(&style_screen_hum_label_main, LV_STATE_DEFAULT, 5);
    lv_style_set_bg_color(&style_screen_hum_label_main, LV_STATE_DEFAULT, lv_color_make(0x6f, 0x82, 0x8b));
    lv_style_set_bg_grad_color(&style_screen_hum_label_main, LV_STATE_DEFAULT, lv_color_make(0x6f, 0x82, 0x85));
    lv_style_set_bg_grad_dir(&style_screen_hum_label_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_hum_label_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_screen_hum_label_main, LV_STATE_DEFAULT, lv_color_make(0x5a, 0xa3, 0xf2));
    lv_style_set_text_font(&style_screen_hum_label_main, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_style_set_text_letter_space(&style_screen_hum_label_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_left(&style_screen_hum_label_main, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_right(&style_screen_hum_label_main, LV_STATE_DEFAULT, 5);
    lv_style_set_pad_top(&style_screen_hum_label_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_screen_hum_label_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui.screen_hum_label, LV_LABEL_PART_MAIN, &style_screen_hum_label_main);
    lv_obj_set_pos(ui.screen_hum_label, 15, 8);
    lv_obj_set_size(ui.screen_hum_label, 290, 0);

    // Write codes screen_hum_graph
    ui.screen_hum_graph = lv_gauge_create(ui.screen_tabview_Humidity, NULL);

    // Write style LV_GAUGE_PART_MAIN for screen_hum_graph
    static lv_style_t style_screen_hum_graph_main;
    lv_style_reset(&style_screen_hum_graph_main);

    // Write style state: LV_STATE_DEFAULT for style_screen_hum_graph_main
    lv_style_set_bg_color(&style_screen_hum_graph_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_screen_hum_graph_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_screen_hum_graph_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_hum_graph_main, LV_STATE_DEFAULT, 255);
    lv_style_set_text_color(&style_screen_hum_graph_main, LV_STATE_DEFAULT, lv_color_make(0x66, 0x00, 0xff));
    lv_style_set_text_font(&style_screen_hum_graph_main, LV_STATE_DEFAULT, lv_theme_get_font_small());
    lv_style_set_text_letter_space(&style_screen_hum_graph_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_inner(&style_screen_hum_graph_main, LV_STATE_DEFAULT, 10);
    lv_style_set_line_color(&style_screen_hum_graph_main, LV_STATE_DEFAULT, lv_color_make(0x8b, 0x89, 0x8b));
    lv_style_set_line_width(&style_screen_hum_graph_main, LV_STATE_DEFAULT, 3);
    lv_style_set_line_opa(&style_screen_hum_graph_main, LV_STATE_DEFAULT, 255);
    lv_style_set_scale_grad_color(&style_screen_hum_graph_main, LV_STATE_DEFAULT, lv_color_make(0x8b, 0x89, 0x8b));
    lv_style_set_scale_end_color(&style_screen_hum_graph_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0xa1, 0xb5));
    lv_style_set_scale_width(&style_screen_hum_graph_main, LV_STATE_DEFAULT, 13);
    lv_style_set_scale_border_width(&style_screen_hum_graph_main, LV_STATE_DEFAULT, 0);
    lv_style_set_scale_end_border_width(&style_screen_hum_graph_main, LV_STATE_DEFAULT, 4);
    lv_style_set_scale_end_line_width(&style_screen_hum_graph_main, LV_STATE_DEFAULT, 4);
    lv_obj_add_style(ui.screen_hum_graph, LV_GAUGE_PART_MAIN, &style_screen_hum_graph_main);

    // Write style LV_GAUGE_PART_MAJOR for screen_hum_graph
    static lv_style_t style_screen_hum_graph_major;
    lv_style_reset(&style_screen_hum_graph_major);

    // Write style state: LV_STATE_DEFAULT for style_screen_hum_graph_major
    lv_style_set_line_color(&style_screen_hum_graph_major, LV_STATE_DEFAULT, lv_color_make(0x8b, 0x89, 0x8b));
    lv_style_set_line_width(&style_screen_hum_graph_major, LV_STATE_DEFAULT, 3);
    lv_style_set_line_opa(&style_screen_hum_graph_major, LV_STATE_DEFAULT, 255);
    lv_style_set_scale_grad_color(&style_screen_hum_graph_major, LV_STATE_DEFAULT, lv_color_make(0x8b, 0x89, 0x8b));
    lv_style_set_scale_end_color(&style_screen_hum_graph_major, LV_STATE_DEFAULT, lv_color_make(0x00, 0xa1, 0xb5));
    lv_style_set_scale_width(&style_screen_hum_graph_major, LV_STATE_DEFAULT, 16);
    lv_style_set_scale_end_line_width(&style_screen_hum_graph_major, LV_STATE_DEFAULT, 5);
    lv_obj_add_style(ui.screen_hum_graph, LV_GAUGE_PART_MAJOR, &style_screen_hum_graph_major);

    // Write style LV_GAUGE_PART_NEEDLE for screen_hum_graph
    static lv_style_t style_screen_hum_graph_needle;
    lv_style_reset(&style_screen_hum_graph_needle);

    // Write style state: LV_STATE_DEFAULT for style_screen_hum_graph_needle
    lv_style_set_size(&style_screen_hum_graph_needle, LV_STATE_DEFAULT, 20);
    lv_style_set_bg_color(&style_screen_hum_graph_needle, LV_STATE_DEFAULT, lv_color_make(0x41, 0x48, 0x5a));
    lv_style_set_bg_grad_color(&style_screen_hum_graph_needle, LV_STATE_DEFAULT, lv_color_make(0x41, 0x48, 0x5a));
    lv_style_set_bg_grad_dir(&style_screen_hum_graph_needle, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_hum_graph_needle, LV_STATE_DEFAULT, 255);
    lv_style_set_pad_inner(&style_screen_hum_graph_needle, LV_STATE_DEFAULT, 0);
    lv_style_set_line_width(&style_screen_hum_graph_needle, LV_STATE_DEFAULT, 2);
    lv_style_set_line_opa(&style_screen_hum_graph_needle, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui.screen_hum_graph, LV_GAUGE_PART_NEEDLE, &style_screen_hum_graph_needle);
    lv_obj_set_pos(ui.screen_hum_graph, 87, 33);
    lv_obj_set_size(ui.screen_hum_graph, 145, 145);
    lv_gauge_set_scale(ui.screen_hum_graph, 270, 21, 11);
    lv_gauge_set_range(ui.screen_hum_graph, 0, 100);
    static lv_color_t needle_colors_screen_hum_graph[1];
    needle_colors_screen_hum_graph[0] = lv_color_make(0xff, 0x00, 0x00);
    lv_gauge_set_needle_count(ui.screen_hum_graph, 1, needle_colors_screen_hum_graph);
    lv_gauge_set_critical_value(ui.screen_hum_graph, 70);
    lv_gauge_set_value(ui.screen_hum_graph, 0, 87);

    // Write codes screen_btn1
    ui.screen_btn1 = lv_btn_create(ui.screen_tabview_Humidity, NULL);

    // Write style LV_BTN_PART_MAIN for screen_btn1
    static lv_style_t style_screen_btn1_main;
    lv_style_reset(&style_screen_btn1_main);

    // Write style state: LV_STATE_DEFAULT for style_screen_btn1_main
    lv_style_set_radius(&style_screen_btn1_main, LV_STATE_DEFAULT, 50);
    lv_style_set_bg_color(&style_screen_btn1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_screen_btn1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_screen_btn1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_btn1_main, LV_STATE_DEFAULT, 255);
    lv_style_set_border_color(&style_screen_btn1_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
    lv_style_set_border_width(&style_screen_btn1_main, LV_STATE_DEFAULT, 2);
    lv_style_set_border_opa(&style_screen_btn1_main, LV_STATE_DEFAULT, 255);
    lv_style_set_outline_color(&style_screen_btn1_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
    lv_style_set_outline_opa(&style_screen_btn1_main, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui.screen_btn1, LV_BTN_PART_MAIN, &style_screen_btn1_main);
    lv_obj_set_pos(ui.screen_btn1, 14, 85);
    lv_obj_set_size(ui.screen_btn1, 65, 27);
    ui.screen_btn1_label = lv_label_create(ui.screen_btn1, NULL);
    lv_label_set_text(ui.screen_btn1_label, "upgrade");
    lv_obj_set_style_local_text_color(ui.screen_btn1_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_obj_set_style_local_text_font(ui.screen_btn1_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_12);

    // Write codes screen_btn2
    ui.screen_btn2 = lv_btn_create(ui.screen_tabview_Humidity, NULL);

    // Write style LV_BTN_PART_MAIN for screen_btn2
    static lv_style_t style_screen_btn2_main;
    lv_style_reset(&style_screen_btn2_main);

    // Write style state: LV_STATE_DEFAULT for style_screen_btn2_main
    lv_style_set_radius(&style_screen_btn2_main, LV_STATE_DEFAULT, 50);
    lv_style_set_bg_color(&style_screen_btn2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_screen_btn2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_screen_btn2_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_btn2_main, LV_STATE_DEFAULT, 255);
    lv_style_set_border_color(&style_screen_btn2_main, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
    lv_style_set_border_width(&style_screen_btn2_main, LV_STATE_DEFAULT, 2);
    lv_style_set_border_opa(&style_screen_btn2_main, LV_STATE_DEFAULT, 255);
    lv_style_set_outline_color(&style_screen_btn2_main, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
    lv_style_set_outline_opa(&style_screen_btn2_main, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui.screen_btn2, LV_BTN_PART_MAIN, &style_screen_btn2_main);
    lv_obj_set_pos(ui.screen_btn2, 240, 85);
    lv_obj_set_size(ui.screen_btn2, 65, 27);
    ui.screen_btn2_label = lv_label_create(ui.screen_btn2, NULL);
    lv_label_set_text(ui.screen_btn2_label, "rollback");
    lv_obj_set_style_local_text_color(ui.screen_btn2_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_obj_set_style_local_text_font(ui.screen_btn2_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_12);

    lv_events_init(&ui);
}
