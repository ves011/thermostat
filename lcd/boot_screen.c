/*
 * boot_screen.c
 *
 *  Created on: Apr 24, 2024
 *      Author: viorel_serbu
 */

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_netif.h"
#include <string.h>
#include <sys/time.h>
#include "project_specific.h"
#include "common_defines.h"
#include "external_defs.h"
#include "lvgl.h"
#include "lcd.h"
#include "state_screen.h"

//#include "rot_enc.h"
//#include "handle_ui_key.h"
#include "boot_screen.h"

static char *TAG = "boot_scr";

extern lv_style_t btn_norm, btn_sel, btn_press, cell_style, cell_style_left;
static lv_obj_t *boot_scr;
static lv_obj_t *bstep[8];

static void draw_boot_screen()
	{
	gpio_set_level(LCD_BK_LIGHT, 0);

	boot_scr = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(boot_scr, lv_color_hex(0xffffff), LV_PART_MAIN);
	int y0 = 5, sy = 22;
	for(int i = 0; i < 8; i++)
		{
		bstep[i] = lv_label_create(boot_scr);
		lv_obj_add_style(bstep[i], &cell_style_left, 0);
		lv_obj_set_pos(bstep[i], 5, y0 + i * sy);
		lv_obj_set_size(bstep[i], 300, 20);
		lv_label_set_text(bstep[i], "");
		}
	_lock_acquire(&lvgl_api_lock);
	lv_scr_load_anim(boot_scr, LV_SCR_LOAD_ANIM_MOVE_LEFT, 100, 0, true);
	_lock_release(&lvgl_api_lock);
	gpio_set_level(LCD_BK_LIGHT, 1);
	}

void do_boot_screen()
	{
	msg_t msg;
	draw_boot_screen();
	char bmsg[80];
	while(1)
		{
		if(xQueueReceive(ui_cmd_q, &msg, portMAX_DELAY))
			{
			if(msg.source == BOOT_MSG)
				{
				ESP_LOGI(TAG, "ui message %u", (unsigned int)msg.val);
				bmsg[0] = 0;
				switch(msg.val)
					{
					case 0:
						strcpy(bmsg, "LCD init completed ");
						break;
					case 1:
						strcpy(bmsg, "spiffs, nvs - initit completed ");
						break;
					case 2:
						strcpy(bmsg, "connected to network ");
						break;
					case 3:
						strcpy(bmsg, "MQTT client started ");
						break;
					case 4:
						strcpy(bmsg, "system commands registered ");
						break;
					case 5:
						strcpy(bmsg, "temperature monitor registered ");
						break;
					case 6:
						strcpy(bmsg, "boot completed ");
						break;
					case 101:			//no temp sensor found
						do_state_screen();
						break;
					}
				if(msg.val == 8)
					break;
				else
					{
					_lock_acquire(&lvgl_api_lock);
					lv_label_set_text(bstep[msg.val], bmsg);
					_lock_release(&lvgl_api_lock);
					}
				}
			//else
			//	break;
			}
		}
	}

