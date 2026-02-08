/*
 * state_screen.c
 *
 *  Created on: Jan 18, 2026
 *      Author: viorel_serbu
 */

#include <stdio.h>
#include "core/lv_obj_style.h"
//#include "esp_netif.h"
#include <string.h>
#include <sys/time.h>
//#include "font/lv_font.h"
//#include "lv_api_map_v8.h"
#include "esp_system.h"
//#include "misc/lv_anim_timeline.h"
#include "misc/lv_color.h"
#include "lv_conf.h"
#include "common_defines.h"
#include "external_defs.h"
#include "temps.h"
#include "rot_enc.h"
#include "ui.h"
#include "lcd.h"
#include "cmd_wifi.h"
#include "mqtt_ctrl.h"
#include "temps.h"
#include "option_screen.h"
#include "state_screen.h"

	
static lv_obj_t * ui_wifinwval = NULL;
static lv_obj_t * ui_wifinwval1 = NULL;
static lv_obj_t * ui_mqttsval = NULL;
static lv_obj_t * ui_tempprobeval = NULL;
static lv_obj_t * ui_tidval = NULL;
	
void draw_state_screen(void)
	{
	lv_obj_t * ui_statescreen = NULL;
	lv_obj_t * ui_wifinw;
	lv_obj_t * ui_mqtts = NULL;
	lv_obj_t * ui_tempprobe = NULL;
	lv_obj_t * ui_tid = NULL;
	
	ui_statescreen = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_statescreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_statescreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_statescreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wifinw = lv_label_create(ui_statescreen);
    lv_obj_set_width(ui_wifinw, 115);
    lv_obj_set_height(ui_wifinw, 15);
    lv_obj_set_x(ui_wifinw, 2);
    lv_obj_set_y(ui_wifinw, 5);
    lv_label_set_text(ui_wifinw, "WiFi:");
    lv_obj_set_style_text_color(ui_wifinw, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wifinw, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_wifinw, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wifinw, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    ui_wifinwval = lv_label_create(ui_statescreen);
    lv_obj_set_width(ui_wifinwval, 190);
    lv_obj_set_height(ui_wifinwval, 18);
    lv_obj_set_x(ui_wifinwval, 125);
    lv_obj_set_y(ui_wifinwval, 5);
    lv_label_set_text(ui_wifinwval, "192.168.1.206");
    lv_obj_set_style_text_color(ui_wifinwval, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wifinwval, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_wifinwval, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wifinwval, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    ui_wifinwval1 = lv_label_create(ui_statescreen);
    lv_obj_set_width(ui_wifinwval1, 190);
    lv_obj_set_height(ui_wifinwval1, 15);
    lv_obj_set_x(ui_wifinwval1, 125);
    lv_obj_set_y(ui_wifinwval1, 30);
    lv_label_set_text(ui_wifinwval1, "192.168.1.206");
    lv_obj_set_style_text_color(ui_wifinwval1, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_wifinwval1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_wifinwval1, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_wifinwval1, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_mqtts = lv_label_create(ui_statescreen);
    lv_obj_set_width(ui_mqtts, 115);
    lv_obj_set_height(ui_mqtts, 15);
    lv_obj_set_x(ui_mqtts, 2);
    lv_obj_set_y(ui_mqtts, 55);
    lv_label_set_text(ui_mqtts, "MQTT server:");
    lv_obj_set_style_text_color(ui_mqtts, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_mqtts, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_mqtts, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_mqtts, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_mqttsval = lv_label_create(ui_statescreen);
    lv_obj_set_width(ui_mqttsval, 190);
    lv_obj_set_height(ui_mqttsval, 15);
    lv_obj_set_x(ui_mqttsval, 125);
    lv_obj_set_y(ui_mqttsval, 55);
    lv_label_set_text(ui_mqttsval, "not connected");
    lv_obj_set_style_text_color(ui_mqttsval, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_mqttsval, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_mqttsval, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_mqttsval, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_tempprobe = lv_label_create(ui_statescreen);
    lv_obj_set_width(ui_tempprobe, 115);
    lv_obj_set_height(ui_tempprobe, 15);
    lv_obj_set_x(ui_tempprobe, 2);
    lv_obj_set_y(ui_tempprobe, 80);
    lv_label_set_text(ui_tempprobe, "Temp Sensor:");
    lv_obj_set_style_text_color(ui_tempprobe, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_tempprobe, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_tempprobe, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_tempprobe, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_tempprobeval = lv_label_create(ui_statescreen);
    lv_obj_set_width(ui_tempprobeval, 190);
    lv_obj_set_height(ui_tempprobeval, 15);
    lv_obj_set_x(ui_tempprobeval, 125);
    lv_obj_set_y(ui_tempprobeval, 80);
    lv_label_set_text(ui_tempprobeval, "not detected");
    lv_obj_set_style_text_color(ui_tempprobeval, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_tempprobeval, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_tempprobeval, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_tempprobeval, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    ui_tid = lv_label_create(ui_statescreen);
    lv_obj_set_width(ui_tid, 115);
    lv_obj_set_height(ui_tid, 18);
    lv_obj_set_x(ui_tid, 2);
    lv_obj_set_y(ui_tid, 105);
    lv_label_set_text(ui_tid, "ID:");
    lv_obj_set_style_text_color(ui_tid, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_tid, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_tid, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_tid, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_tidval = lv_label_create(ui_statescreen);
    lv_obj_set_width(ui_tidval, 190);
    lv_obj_set_height(ui_tidval, 18);
    lv_obj_set_x(ui_tidval, 125);
    lv_obj_set_y(ui_tidval, 105);
    lv_label_set_text(ui_tidval, "id");
    lv_obj_set_style_text_color(ui_tidval, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_tidval, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_tidval, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_tidval, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    _lock_acquire(&lvgl_api_lock);
	lv_scr_load_anim(ui_statescreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 100, 0, false);
	_lock_release(&lvgl_api_lock);
    }
    
void do_state_screen()
	{
	msg_t msg;
	draw_state_screen();
	xQueueReset(ui_cmd_q);
	char ssid[64], ip[64];
	esp_netif_ip_info_t ipinfo;
	uint64_t hwad;
	if(get_sta_conf(ssid, &ipinfo))
		{
		sprintf(ip, IPSTR, IP2STR(&ipinfo.ip));
		lv_label_set_text(ui_wifinwval, ssid);
		lv_label_set_text(ui_wifinwval1, ip);
		lv_obj_set_style_text_color(ui_wifinwval, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_text_color(ui_wifinwval1, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
		}
	else
		{
		lv_label_set_text(ui_wifinwval, "Not connected");
		lv_label_set_text(ui_wifinwval1, "");
		lv_obj_set_style_text_color(ui_wifinwval, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_text_color(ui_wifinwval1, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
		}
	if(get_MQTT_connection_state(ssid))
		{
		lv_label_set_text(ui_mqttsval, "connected");
		lv_obj_set_style_text_color(ui_mqttsval, lv_color_hex(0x00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
		}
	else
		{
		lv_label_set_text(ui_mqttsval, "not connected");
		lv_obj_set_style_text_color(ui_mqttsval, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
		}
	lv_label_set_text(ui_tidval, ssid);
	if((hwad = get_t_hwadd(0)) != 0)
		{
		sprintf(ssid, "%llX", hwad);
		lv_label_set_text(ui_tempprobeval, ssid);
		lv_obj_set_style_text_color(ui_tempprobeval, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
		}
	else 
		{
		lv_label_set_text(ui_tempprobeval, "not detected");
		lv_obj_set_style_text_color(ui_tempprobeval, lv_color_hex(0x0000FF), LV_PART_MAIN | LV_STATE_DEFAULT);
		}
	
	
	
	while(1)
		{
		if(xQueueReceive(ui_cmd_q, &msg, portMAX_DELAY))
			{
			switch(msg.source)
				{
				case K_PRESS:
					if(hwad == 0)
						esp_restart();
					return;
					break;
				default:
					break;
				}
			}
		}
	}