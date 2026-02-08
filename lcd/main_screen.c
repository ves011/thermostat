/*
 * main_screen.c
 *
 *  Created on: Jan 10, 2026
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
#include "misc/lv_anim_timeline.h"
#include "misc/lv_color.h"
#include "driver/mcpwm_prelude.h"
//#include "soc/mcpwm_periph.h"
#include "driver/mcpwm_timer.h"
#include "esp_clk_tree.h"
//#include "driver/mcpwm.h"
#include "project_specific.h"
#include "common_defines.h"
#include "external_defs.h"
#include "config.h"
#include "temps.h"
#include "rot_enc.h"
#include "mqtt_ctrl.h"
#include "ui.h"
#include "option_screen.h"
#include "lcd.h"

#include "main_screen.h"

static char *TAG = "main_scr";

//extern lv_style_t btn_norm, btn_sel, btn_press, cell_style, cell_style_left;
//static lv_obj_t *main_scr;
//static lv_obj_t *temp_val;

//lv_obj_t * ui_mainscreen = NULL;
lv_obj_t * ui_acttemp = NULL;
//lv_obj_t * ui_heating = NULL;
lv_obj_t * ui_trange = NULL;
lv_obj_t * ui_mint = NULL;
lv_obj_t * ui_maxt = NULL;
lv_obj_t * ui_dtime = NULL;
lv_obj_t * ui_nwstate = NULL;

// IMAGES AND IMAGE SETS
//const lv_image_dsc_t * ui_imgset_frame_[6] = {&ui_img_frame_00_png, &ui_img_frame_01_png, &ui_img_frame_02_png, &ui_img_frame_03_png, &ui_img_frame_04_png, &ui_img_frame_05_png};

#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif


//lv_anim_t PropertyAnimation_0, PropertyAnimation_1;

lv_anim_t tempset_anim, *ts_anim;
lv_anim_timeline_t *timeline_heat;
lv_obj_t * ui_mainscreen = NULL;


//extern float	target_temp, hist_temp, freeze_temp;
//extern int		on_off_time, maint_time;

#define LOWTXT_COLOR		0xffc0c0
#define HIGHTXT_COLOR		0xffff00
#define COLOR_STEP 			(HIGHTXT_COLOR - LOWTXT_COLOR) / 100
	



//void anim_completed_cb(lv_anim_t *a)
//	{
	//ESP_LOGI(TAG, "anim completed");
	//out_anim = lv_anim_start(&PropertyAnimation_0);
//	}

void tempset_Animation(lv_obj_t * TargetObject, int delay)
	{
    ui_anim_user_data_t * PropertyAnimation_0_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    
    lv_anim_init(&tempset_anim);
    lv_anim_set_time(&tempset_anim, 1000);
    lv_anim_set_user_data(&tempset_anim, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&tempset_anim, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&tempset_anim, 0, 255);
    lv_anim_set_path_cb(&tempset_anim, lv_anim_path_linear);
    lv_anim_set_delay(&tempset_anim, delay + 0);
    lv_anim_set_deleted_cb(&tempset_anim, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&tempset_anim, 0);
    lv_anim_set_playback_delay(&tempset_anim, 0);
    lv_anim_set_repeat_count(&tempset_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&tempset_anim, 0);
    lv_anim_set_early_apply(&tempset_anim, false);
    lv_anim_set_get_value_cb(&tempset_anim, &_ui_anim_callback_get_opacity);
	}

lv_anim_t * heat_Animation(lv_obj_t * TargetObject, int delay)
	{
    //lv_anim_t * out_anim;
    ui_anim_user_data_t * PropertyAnimation_0_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 3000);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_out);//lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);//LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    
    ui_anim_user_data_t * PropertyAnimation_1_user_data = lv_malloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_1_user_data->target = TargetObject;
    PropertyAnimation_1_user_data->val = -1;
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 3000);
    lv_anim_set_user_data(&PropertyAnimation_1, PropertyAnimation_1_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_1, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_ease_in);//lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_1, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_1, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_1, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_1, 0);//LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&PropertyAnimation_1, 0);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity);
    //out_anim = lv_anim_start(&PropertyAnimation_1);
    
	timeline_heat = lv_anim_timeline_create();
	lv_anim_timeline_add(timeline_heat, 0, &PropertyAnimation_0);
	lv_anim_timeline_add(timeline_heat, 4000, &PropertyAnimation_1);
	lv_anim_timeline_set_repeat_count(timeline_heat, LV_ANIM_REPEAT_INFINITE);

    return 0;
	}

static void draw_main_screen()
	{
	char buf[32];
	
	lv_obj_t * ui_heating = NULL;
	gpio_set_level(LCD_BK_LIGHT, 0);
	/*
	lv_disp_t * dispp = lv_display_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    */

	ui_mainscreen = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_mainscreen, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_SCROLLABLE |
                       LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM);     /// Flags
    lv_obj_set_style_bg_color(ui_mainscreen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_mainscreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    time_t now = time(NULL);
	struct tm timeinfo;
	localtime_r(&now, &timeinfo);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &timeinfo);
					
    ui_dtime = lv_label_create(ui_mainscreen);
    lv_obj_set_width(ui_dtime, 140); 
    lv_obj_set_height(ui_dtime, 12); 
    lv_obj_set_x(ui_dtime, 5);
    lv_obj_set_y(ui_dtime, 10);
    lv_label_set_text(ui_dtime, buf);
    lv_obj_set_style_text_color(ui_dtime, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_dtime, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_dtime, &ui_font_segblack16, LV_PART_MAIN | LV_STATE_DEFAULT);
    
    ui_nwstate = lv_image_create(ui_mainscreen);
    lv_image_set_src(ui_nwstate, &ui_img_wifi_png);
    lv_obj_set_width(ui_nwstate, 33);
    lv_obj_set_height(ui_nwstate, 30);    /// 20
    lv_obj_set_x(ui_nwstate, 282);
    lv_obj_set_y(ui_nwstate, 0);
    lv_image_set_scale(ui_nwstate, 255);
    lv_image_set_inner_align(ui_nwstate, LV_IMAGE_ALIGN_TOP_LEFT);
    

    ui_acttemp = lv_label_create(ui_mainscreen);
    lv_obj_set_width(ui_acttemp, 220);
    lv_obj_set_height(ui_acttemp, 59);
    lv_obj_set_x(ui_acttemp, 50);
    lv_obj_set_y(ui_acttemp, 30);
	sprintf(buf, "%.1f °C", target_temp);
    lv_label_set_text(ui_acttemp, buf);
    lv_obj_set_style_text_color(ui_acttemp, lv_color_hex(0xffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_acttemp, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_acttemp, &ui_font_hemihead60, LV_PART_MAIN | LV_STATE_DEFAULT);
	
	ui_trange = lv_slider_create(ui_mainscreen);
    lv_slider_set_value(ui_trange, 50, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_trange) == LV_SLIDER_MODE_RANGE) 
    	lv_slider_set_left_value(ui_trange, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_trange, 260);
    lv_obj_set_height(ui_trange, 10);
    lv_obj_set_x(ui_trange, 30);
    lv_obj_set_y(ui_trange, 110);
    lv_obj_remove_flag(ui_trange, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                       LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE);     /// Flags
    //lv_obj_set_style_bg_color(ui_trange, lv_color_hex(0x102CB5), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_trange, lv_color_hex(0xffc0c0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_trange, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_grad_color(ui_trange, lv_color_hex(0x8B5BAE), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_trange, lv_color_hex(0xffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_trange, LV_GRAD_DIR_HOR, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_trange, 50, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_trange, lv_color_hex(0xffc0c0), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_trange, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_trange, lv_color_hex(0xffff00), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui_trange, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_trange, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_trange, LV_GRAD_DIR_HOR, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui_trange, false, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_trange, 5, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_trange, lv_color_hex(0xffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_trange, 255, LV_PART_KNOB | LV_STATE_DEFAULT);

    //Compensating for LVGL9.1 draw crash with bar/slider max value when top-padding is nonzero and right-padding is 0
    if(lv_obj_get_style_pad_top(ui_trange, LV_PART_MAIN) > 0)
    	{ 
    	lv_obj_set_style_pad_right(ui_trange, lv_obj_get_style_pad_right(ui_trange, LV_PART_MAIN) + 1, LV_PART_MAIN);
    	}

	ui_mint = lv_label_create(ui_mainscreen);
    lv_obj_set_width(ui_mint, 44);
    lv_obj_set_height(ui_mint, 16);
    lv_obj_set_x(ui_mint, 8);
    lv_obj_set_y(ui_mint, 134);
    sprintf(buf, "%.1f", target_temp - hist_temp);
    lv_label_set_text(ui_mint, buf);
    lv_obj_set_style_text_color(ui_mint, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_mint, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_mint, &ui_font_segblack20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_maxt = lv_label_create(ui_mainscreen);
    lv_obj_set_width(ui_maxt, 44);
    lv_obj_set_height(ui_maxt, 15);
    lv_obj_set_x(ui_maxt, 268);
    lv_obj_set_y(ui_maxt, 134);
    sprintf(buf, "%.1f", target_temp + hist_temp);
    lv_label_set_text(ui_maxt, buf);
    lv_obj_set_style_text_color(ui_maxt, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_maxt, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_maxt, &ui_font_segblack20, LV_PART_MAIN | LV_STATE_DEFAULT);
    
	ui_heating = lv_image_create(ui_mainscreen);
    lv_image_set_src(ui_heating, &ui_img_frame_00_png);
    lv_obj_set_width(ui_heating, 60);
    lv_obj_set_height(ui_heating, 80);
    lv_obj_set_x(ui_heating, 130);
    lv_obj_set_y(ui_heating, 150);

	heat_Animation(ui_heating, 0);
	tempset_Animation(ui_acttemp, 0);
	
	//_lock_acquire(&lvgl_api_lock);
	//lv_scr_load_anim(ui_mainscreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 100, 0, true);
	//_lock_release(&lvgl_api_lock);

	gpio_set_level(LCD_BK_LIGHT, 1);
	}
static uint32_t bl_period = 5000, bl_ff = 90, bl_tick = 0;	
mcpwm_cmpr_handle_t comparator = NULL;
void set_pwm();
static struct
	{
    struct arg_str *op;
    struct arg_int *arg;
    struct arg_end *end;
	} ms_args;

int do_mainscr_console(int argc, char **argv)
	{
	msg_t msg;
	int nerrors = arg_parse(argc, argv, (void **)&ms_args);
	if (nerrors != 0)
		{
		arg_print_errors(stderr, ms_args.end, argv[0]);
		return ESP_FAIL;
		}
	if(strcmp(ms_args.op->sval[0], "nw") == 0)
		{
		if(ms_args.arg->count)
			{
			msg.source = NW_STATE;
			msg.val = ms_args.arg->ival[0];
			xQueueSend(ui_cmd_q, &msg, pdMS_TO_TICKS(20)); 
			} 
		}
	else if(strcmp(ms_args.op->sval[0], "t") == 0)
		{
		if(ms_args.arg->count)
			{
			msg.source = ACT_TEMP;
			msg.val = ms_args.arg->ival[0];
			xQueueSend(ui_cmd_q, &msg, pdMS_TO_TICKS(20)); 
			} 
		}
	else if(strcmp(ms_args.op->sval[0], "tr1") == 0)
		{
		if(ms_args.arg->count)
			{
			msg.source = TMIN_VAL;
			msg.val = ms_args.arg->ival[0];
			xQueueSend(ui_cmd_q, &msg, pdMS_TO_TICKS(20)); 
			} 
		}
	else if(strcmp(ms_args.op->sval[0], "tr2") == 0)
		{
		if(ms_args.arg->count)
			{
			msg.source = TMAX_VAL;
			msg.val = ms_args.arg->ival[0];
			xQueueSend(ui_cmd_q, &msg, pdMS_TO_TICKS(20)); 
			} 
		}
	else if(strcmp(ms_args.op->sval[0], "h") == 0)
		{
		if(ms_args.arg->count)
			{
			msg.source = HEATING_ONOFF;
			msg.val = ms_args.arg->ival[0];
			xQueueSend(ui_cmd_q, &msg, pdMS_TO_TICKS(20)); 
			} 
		}
	else if(strcmp(ms_args.op->sval[0], "ts") == 0)
		{
		if(ms_args.arg->count)
			{
			msg.source = TEMP_SET;
			msg.val = ms_args.arg->ival[0];
			xQueueSend(ui_cmd_q, &msg, pdMS_TO_TICKS(20)); 
			} 
		}
	else if(strcmp(ms_args.op->sval[0], "blp") == 0)
		{
		if(ms_args.arg->count)
			{
			bl_period = ms_args.arg->ival[0];
			} 
		}
	else if(strcmp(ms_args.op->sval[0], "blf") == 0)
		{
		if(ms_args.arg->count)
			{
			bl_ff = ms_args.arg->ival[0];
			//ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, bl_ff * 50));
			} 
		}
	return 0;
	}
void register_mainscr_console()
	{
	ms_args.op = arg_str1(NULL, NULL, "<op>", "op: r | s");
	ms_args.arg= arg_int0(NULL, NULL, "<arg>", "set sensor #");
	ms_args.end = arg_end(1);
	const esp_console_cmd_t ms_cmd =
		{	
		.command = "ms",
		.help = "read (r) / write (w) to/from AD",
		.hint = NULL,
		.func = &do_mainscr_console,
		.argtable = &ms_args
		};
	ESP_ERROR_CHECK(esp_console_cmd_register(&ms_cmd));
	}
static void display_values(float t, float tmin_val, float tmax_val, int animation)
	{
	char buf[20];
	_lock_acquire(&lvgl_api_lock);
	sprintf(buf, "%.1f °C", t);
	lv_label_set_text(ui_acttemp, buf);
	sprintf(buf, "%.1f", tmax_val);
	lv_label_set_text(ui_maxt, buf);
	sprintf(buf, "%.1f", tmin_val);
	lv_label_set_text(ui_mint, buf);
	int pos = (int)((t - tmin_val) * 100. / (tmax_val - tmin_val));
	if(pos > 100)
		pos = 100;
	if(pos < 0) 
		pos = 0;
	lv_slider_set_value(ui_trange, pos, LV_ANIM_OFF);
	uint32_t color = LOWTXT_COLOR + pos * COLOR_STEP;
	lv_obj_set_style_text_color(ui_acttemp, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
	
	if(animation == 1)
		ts_anim = lv_anim_start(&tempset_anim);
	else if(animation == 0)
 		{
		if(ts_anim)
			{
			lv_anim_pause(ts_anim);
			lv_obj_set_style_opa(ui_acttemp, 255, 0);
			}
		}
	
	_lock_release(&lvgl_api_lock);
	}
	
static void bl_timer_callback(void *args)
	{
	bl_tick++;
	bl_tick %= bl_period;
	if(bl_tick == bl_ff)
		gpio_set_level(LCD_BK_LIGHT, 0);
	if(bl_tick == 0)
		gpio_set_level(LCD_BK_LIGHT, 1);
	}
void do_main_screen(int active_screen)
	{
	msg_t msg;
	int therm_state = MEASURING_STATE;
	int nw_connection_state = get_MQTT_connection_state(NULL);
	float t;
	float starget_temp = 0;
	if(ui_mainscreen == NULL)
		draw_main_screen();
	
	_lock_acquire(&lvgl_api_lock);
	lv_scr_load_anim(ui_mainscreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 100, 0, false);
	if(nw_connection_state == 0)
		lv_image_set_src(ui_nwstate, &ui_img_wifi_error_png);
	_lock_release(&lvgl_api_lock);
	
	xQueueReset(ui_cmd_q);
	
	char buf[40];
	int dt = 0;
	register_mainscr_console();	
	set_inactivity = 0; inactivity_set_sec = 0;
	_lock_acquire(&lvgl_api_lock);
	lv_anim_timeline_set_progress(timeline_heat, 32767);
	lv_anim_timeline_pause(timeline_heat);
	_lock_release(&lvgl_api_lock);
	msg.source = MSG_TEMP_DATA;
	xQueueSend(temp_mon_queue, &msg, pdMS_TO_TICKS(20));
	msg.source = MSG_TEMP_DATA;
	xQueueSend(temp_mon_queue, &msg, pdMS_TO_TICKS(20));
	//get_temp(0, &t);
	//display_values(t, target_temp - hist_temp, target_temp + hist_temp, 0);
	lcd_inactivity = 0;
	//set_pwm();
	/*
	const esp_timer_create_args_t clock_timer_args =
		{
         .callback = &bl_timer_callback,
         .name = ""
		};
	esp_timer_handle_t bl_timer;
    ESP_ERROR_CHECK(esp_timer_create(&clock_timer_args, &bl_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(bl_timer, 2000));
	*/
	while(1)
		{
		if(xQueueReceive(ui_cmd_q, &msg, portMAX_DELAY))
			{
			ESP_LOGI(TAG, "main screen msg.source = %d", (int)msg.source);
			switch(msg.source)
				{
				case CLOCK_TICK_1M:
					{
					time_t now = time(NULL);
					struct tm timeinfo;
	    			char strtime[50];
					localtime_r(&now, &timeinfo);
					strftime(strtime, sizeof(strtime), "%Y-%m-%d %H:%M", &timeinfo);
					sprintf(buf, "inactivity timer: %d", dt++);
					int cs = get_MQTT_connection_state(NULL);
					_lock_acquire(&lvgl_api_lock);
					lv_label_set_text(ui_dtime, strtime);
					if(cs != nw_connection_state)
						{
						if(cs == 0)
							lv_image_set_src(ui_nwstate, &ui_img_wifi_error_png);
						else
							lv_image_set_src(ui_nwstate, &ui_img_wifi_png);
						nw_connection_state = cs;
						}
					_lock_release(&lvgl_api_lock);					
					break;
					}
				case INACTIVE_LCD:
					{
					gpio_set_level(LCD_BK_LIGHT, 0);
					break;
					}
				case INACTIVE_SET:
					if(therm_state == SET_STATE)
						{
						get_temp(0, &t);
						display_values(t, target_temp - hist_temp, target_temp + hist_temp, 0);
		    			therm_state = MEASURING_STATE;
		    			msg.source = MSG_TEMP_DATA;
	    				xQueueSend(temp_mon_queue, &msg, pdMS_TO_TICKS(20));
						}
					break;
				case ACT_TEMP:
					{
					if(therm_state == MEASURING_STATE)
						{
						t = (float)msg.val / 100.;
						display_values(t, target_temp - hist_temp, target_temp + hist_temp, 0);
						}
					break;
					}
				case HEATING_ONOFF:
					_lock_acquire(&lvgl_api_lock);
					if(msg.val == 1)
						{
						lv_anim_timeline_set_progress(timeline_heat, 0);
						lv_anim_timeline_start(timeline_heat); 
						}
					else
						{
						lv_anim_timeline_set_progress(timeline_heat, 32767);
						lv_anim_timeline_pause(timeline_heat);
						}
					_lock_release(&lvgl_api_lock);
					break;
				case K_PRESS:
					{
					inactivity_set_sec = 0;	
					gpio_set_level(LCD_BK_LIGHT, 1);
					inactivity_lcd_sec = 0;
					if(msg.ifvals.ival[0] == 3)
						{
						ESP_LOGI(TAG, "key pressed 3 times");
						do_option_screen();
						_lock_acquire(&lvgl_api_lock);
						//lv_anim_delete_all();
						lv_scr_load_anim(ui_mainscreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 100, 0, false);
						_lock_release(&lvgl_api_lock);
						break;
						}
					if(msg.val == PUSH_TIME_LONG)
						{
						if(therm_state == MEASURING_STATE) // enter set state
							{
							starget_temp = target_temp;
							display_values(starget_temp, starget_temp - hist_temp, starget_temp + hist_temp, 1);
							therm_state = SET_STATE;
							// start inactivity set
							set_inactivity = 1;
							// hide heating icon
							//_lock_acquire(&lvgl_api_lock);
							//lv_anim_timeline_set_progress(timeline_heat, 32767);
							//lv_anim_timeline_pause(timeline_heat);
							//_lock_release(&lvgl_api_lock);
							}
						else if(therm_state == SET_STATE) // exit set state
							{
							// save target temp set before (if changed)
							if(starget_temp != target_temp)
								set_configuration((int)(starget_temp * 100.), (int)(hist_temp * 100.), (int)(freeze_temp * 100.), on_off_time, maint_time);
							get_temp(0, &t);
							display_values(t, target_temp - hist_temp, target_temp + hist_temp, 0);
	    					therm_state = MEASURING_STATE;
	    					set_inactivity = 0;
	    					msg.source = MSG_TEMP_DATA;
	    					xQueueSend(temp_mon_queue, &msg, pdMS_TO_TICKS(20));
							}
						}
					break;
					}
				case K_ROT:
					inactivity_lcd_sec = 0;
					inactivity_set_sec = 0;
					gpio_set_level(LCD_BK_LIGHT, 1);
					if(therm_state == SET_STATE)
						{
						
						if(msg.val == K_ROT_LEFT && starget_temp > 10.)
							{
							starget_temp -= 0.1;
							display_values(starget_temp, starget_temp - hist_temp, starget_temp + hist_temp, 2);
		    				}
						if(msg.val == K_ROT_RIGHT && starget_temp < 28.)
							{
							starget_temp += 0.1;
							display_values(starget_temp, starget_temp - hist_temp, starget_temp + hist_temp, 2);
		    				}
						}
					break;
				default:
					break;
				}
			}
		}
	}	
/*
void set_pwm()
	{
    ESP_LOGI(TAG, "Create timer and operator");
    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = 
    	{
        .group_id = 0,
        //.clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .period_ticks = bl_period,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    	};
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    mcpwm_oper_handle_t oper = NULL;
    mcpwm_operator_config_t operator_config = {
        .group_id = 0, // operator must be in the same group to the timer
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &oper));

    ESP_LOGI(TAG, "Connect timer and operator");
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

    ESP_LOGI(TAG, "Create comparator and generator from the operator");
    
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &comparator_config, &comparator));

    mcpwm_gen_handle_t generator = NULL;
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = LCD_BK_LIGHT,
    };
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &generator_config, &generator));

    // set the initial compare value, so that the servo will spin to the center position
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, bl_ff * 50));

    ESP_LOGI(TAG, "Set generator action on timer and compare event");
    // go high on counter empty
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generator,
                                                              MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
    // go low on compare threshold
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generator,
                                                                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW)));

    ESP_LOGI(TAG, "Enable and start timer");
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    //ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, bl_ff * 50));
	}
*/

/*
			if(msg.source == TMIN_VAL)
				{
				tmin_val = (float)msg.val / 100.;
				sprintf(buf, "%.1f", tmin_val);
				_lock_acquire(&lvgl_api_lock);
				lv_label_set_text(ui_mint, buf);
				_lock_release(&lvgl_api_lock);
				}
			if(msg.source == TMAX_VAL)
				{
				tmax_val = (float)msg.val / 100.;
				sprintf(buf, "%.1f", tmax_val);
				_lock_acquire(&lvgl_api_lock);
				lv_label_set_text(ui_maxt, buf);
				_lock_release(&lvgl_api_lock);
				}
			if(msg.source == TMAX_VAL || msg.source == TMIN_VAL)
				{
				// update ui_trange knob position
				int pos = (int)((act_t - tmin_val) * 100. / (tmax_val - tmin_val));
				if(pos > 100)pos = 100;
				if(pos < 0) pos = 0;
				_lock_acquire(&lvgl_api_lock);
				lv_slider_set_value(ui_trange, pos, LV_ANIM_OFF);
				uint32_t color = LOWTXT_COLOR + pos * COLOR_STEP;
				lv_obj_set_style_text_color(ui_acttemp, lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
				_lock_release(&lvgl_api_lock);
				}
			if(msg.source == HEATING_ONOFF)
				{
				// start or stop animation on heating icon
				_lock_acquire(&lvgl_api_lock);
				if(msg.val == 1)
					{
					lv_anim_timeline_set_progress(timeline_heat, 0);
					lv_anim_timeline_start(timeline_heat);
					}
				else
					{
					lv_anim_timeline_set_progress(timeline_heat, 32767);
					lv_anim_timeline_pause(timeline_heat);
					}
				_lock_release(&lvgl_api_lock);
				}
			if(msg.source == TEMP_SET)
				{
				// start act temp blinking
				_lock_acquire(&lvgl_api_lock);
				if(msg.val == 1)
					{
					ts_anim = lv_anim_start(&tempset_anim);
					}
				else
					{
					if(ts_anim)
						{
						lv_anim_pause(ts_anim);
						lv_obj_set_style_opa(ui_acttemp, 255, 0);
						}
					}
				_lock_release(&lvgl_api_lock);
				}
			*/