/*
 * option_screen.c
 *
 *  Created on: Jan 17, 2026
 *      Author: viorel_serbu
 */
 
#include <stdio.h>
#include "core/lv_obj_style.h"
#include "driver/gpio.h"
#include "esp_log.h"
//#include "esp_netif.h"
#include <string.h>
#include <sys/time.h>
#include "esp_console.h"
#include "argtable3/argtable3.h"
//#include "font/lv_font.h"
//#include "lv_api_map_v8.h"
#include "lv_api_map_v9_1.h"
#include "misc/lv_anim.h"
//#include "misc/lv_anim_timeline.h"
#include "misc/lv_color.h"
#include "lv_conf.h"
#include "project_specific.h"
#include "common_defines.h"
#include "external_defs.h"
#include "config.h"
#include "temps.h"
#include "rot_enc.h"
#include "ui.h"
#include "lcd.h"
#include "state_screen.h"
#include "option_screen.h"

lv_obj_t *ui_label[4];
lv_anim_t opt_anim[4];
lv_obj_t *ui_optionscreen = NULL;

lv_anim_t * label_Animation(int idx)
	{
    //lv_anim_t * out_anim;
    ui_anim_user_data_t * PropertyAnimation_0_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = ui_label[idx];
    PropertyAnimation_0_user_data->val = -1;
    //lv_anim_t PropertyAnimation_0;
    lv_anim_init(&opt_anim[idx]); 
    lv_anim_set_time(&opt_anim[idx], 1000);
    lv_anim_set_user_data(&opt_anim[idx], PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&opt_anim[idx], _ui_anim_callback_set_opacity);
    lv_anim_set_values(&opt_anim[idx], 0, 255);
    lv_anim_set_path_cb(&opt_anim[idx], lv_anim_path_linear);
    lv_anim_set_delay(&opt_anim[idx], 0);
    lv_anim_set_deleted_cb(&opt_anim[idx], _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&opt_anim[idx], 0);
    lv_anim_set_playback_delay(&opt_anim[idx], 0);
    lv_anim_set_repeat_count(&opt_anim[idx], LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&opt_anim[idx], 0);
    lv_anim_set_early_apply(&opt_anim[idx], false);
    lv_anim_set_get_value_cb(&opt_anim[idx], &_ui_anim_callback_get_opacity);
    //out_anim = lv_anim_start(&opt_anim[idx]);

    return NULL;
	}


static void draw_option_screen()
	{
	ui_optionscreen = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_optionscreen, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_optionscreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_optionscreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_t *ui_state = lv_label_create(ui_optionscreen);
    lv_obj_set_width(ui_state, 162);
    lv_obj_set_height(ui_state, 40);
    lv_obj_set_x(ui_state, 82);
    lv_obj_set_y(ui_state, 10);
    lv_label_set_text(ui_state, "stare");
    lv_obj_set_style_text_color(ui_state, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_state, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_state, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_state, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
	ui_label[0] = ui_state;
		
    lv_obj_t *ui_restartop = lv_label_create(ui_optionscreen);
    lv_obj_set_width(ui_restartop, 162);
    lv_obj_set_height(ui_restartop, 40);
    lv_obj_set_x(ui_restartop, 82);
    lv_obj_set_y(ui_restartop, 50);
    lv_label_set_text(ui_restartop, "restart");
    lv_obj_set_style_text_color(ui_restartop, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_restartop, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_restartop, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_restartop, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
	ui_label[1] = ui_restartop;
		
    lv_obj_t *ui_bootota = lv_label_create(ui_optionscreen);
    lv_obj_set_width(ui_bootota, 162);
    lv_obj_set_height(ui_bootota, 40);
    lv_obj_set_x(ui_bootota, 82);
    lv_obj_set_y(ui_bootota, 90);
    lv_label_set_text(ui_bootota, "OTA");
    lv_obj_set_style_text_color(ui_bootota, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_bootota, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_bootota, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_bootota, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
	ui_label[2] = ui_bootota;
	 
    lv_obj_t *ui_cancel = lv_label_create(ui_optionscreen);
    lv_obj_set_width(ui_cancel, 162);
    lv_obj_set_height(ui_cancel, 40);
    lv_obj_set_x(ui_cancel, 82);
    lv_obj_set_y(ui_cancel, 175);
    lv_label_set_text(ui_cancel, "inapoi");
    lv_obj_set_style_text_color(ui_cancel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_cancel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_cancel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_cancel, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_label[3] = ui_cancel;
    
    for(int i = 0; i < 4; i++)
    	label_Animation(i);
        
    
	
	}

void do_option_screen()
	{
	msg_t msg;
	int opt_sel = 3;
	lv_anim_t *anim;
	xQueueReset(ui_cmd_q);
	if(!ui_optionscreen)
		draw_option_screen();
	_lock_acquire(&lvgl_api_lock);
	lv_scr_load_anim(ui_optionscreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 100, 0, false);
	_lock_release(&lvgl_api_lock);
	anim = lv_anim_start(&opt_anim[opt_sel]);
	_lock_release(&lvgl_api_lock);
	//char buf[40];

	while(1)
		{
		if(xQueueReceive(ui_cmd_q, &msg, portMAX_DELAY))
			{
			switch(msg.source)
				{
				case K_PRESS:
					if(opt_sel == 3)
						{
						if(ui_optionscreen) 
							{
							_lock_acquire(&lvgl_api_lock);
							lv_anim_pause(anim);
							//lv_obj_del(ui_optionscreen);
							//ui_optionscreen = NULL;
							_lock_release(&lvgl_api_lock);
							}
						return;
						}
					else if(opt_sel == 0)
						{
						do_state_screen();
						//draw_option_screen();
						 _lock_acquire(&lvgl_api_lock);
						lv_scr_load_anim(ui_optionscreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 100, 0, false);
						_lock_release(&lvgl_api_lock);

						//_lock_acquire(&lvgl_api_lock);
						//anim = lv_anim_start(&opt_anim[opt_sel]);
						//_lock_release(&lvgl_api_lock);
						}
					break;
				case K_ROT:
					_lock_acquire(&lvgl_api_lock);
					if(anim)
						{
						lv_anim_pause(anim);
						lv_obj_set_style_opa(ui_label[opt_sel], 255, 0);
						}
					if(msg.val == K_ROT_LEFT)
						opt_sel--;
					else
 						opt_sel++;
					if(opt_sel < 0) opt_sel = 3;
					if(opt_sel > 3) opt_sel = 0;
					anim = lv_anim_start(&opt_anim[opt_sel]);
					_lock_release(&lvgl_api_lock);
					break;
				}
			}
		}
	//lv_obj_del(ui_optionscreen);
	//ui_optionscreen = NULL;
	}