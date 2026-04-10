/**
 * @file main_screen.c
 * @brief Main thermostat operational UI screen
 *
 * This module implements the primary user interface of the thermostat
 * during normal operation. It is responsible for:
 *
 *  - Displaying current temperature and target range
 *  - Displaying system date/time (SNTP-based)
 *  - Indicating network connectivity state
 *  - Indicating heating activity via animations
 *  - Handling user interaction (rotary encoder + button)
 *  - Managing UI-level state transitions (MEASURING vs SET)
 *
 * The screen is modal and event-driven. It consumes messages from ui_cmd_q
 * and does not return unless a transition to another screen is requested.
 *
 * Design notes:
 *  - Animations are created once and live for the lifetime of the screen.
 *  - All LVGL API calls are protected using lvgl_api_lock.
 *  - No actuator or control logic is executed from this module.
 */

#include <stdio.h>
#include "core/lv_obj_style.h"
#include "driver/gpio.h"
#include <string.h>
#include <sys/time.h>
#include "misc/lv_anim.h"
#include "misc/lv_anim_timeline.h"
#include "misc/lv_color.h"
#include "project_specific.h"
#include "common_defines.h"
#include "temps.h"
#include "rot_enc.h"
#include "mqtt_ctrl.h"
#include "ui.h"
#include "option_screen.h"
#include "lcd.h"

#include "main_screen.h"

/* -------------------------------------------------------------
 * LVGL object references
 * ------------------------------------------------------------- */

/** Current temperature label */
static lv_obj_t *ui_acttemp = NULL;

/** Temperature range slider */
static lv_obj_t *ui_trange = NULL;

/** Minimum target temperature label */
static lv_obj_t *ui_mint = NULL;

/** Maximum target temperature label */
static lv_obj_t *ui_maxt = NULL;

/** Date/time label */
static lv_obj_t *ui_dtime = NULL;

/** Network status icon */
static lv_obj_t *ui_nwstate = NULL;

#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif

/* -------------------------------------------------------------
 * Animations and screen objects
 * ------------------------------------------------------------- */

/**
 * @brief Blink animation used while in SET mode
 *
 * The animation is created once and reused whenever SET mode
 * is entered. It indicates that the user is adjusting the target
 * temperature.
 */
lv_anim_t tempset_anim;

/** Pointer to active instance of tempset animation */
lv_anim_t *ts_anim;

/**
 * @brief Heating activity animation timeline
 *
 * This animation runs while heating is ON and is paused while heating is OFF.
 */
lv_anim_timeline_t *timeline_heat;

/** Root LVGL object for the main screen */
lv_obj_t *ui_mainscreen = NULL;

/* -------------------------------------------------------------
 * Color configuration
 * ------------------------------------------------------------- */

/** Low-end color for temperature interpolation */
#define LOWTXT_COLOR   0xffc0c0

/** High-end color for temperature interpolation */
#define HIGHTXT_COLOR  0xffff00

/** Per-percent interpolation step */
#define COLOR_STEP     ((HIGHTXT_COLOR - LOWTXT_COLOR) / 100)

/* -------------------------------------------------------------
 * Animation helpers
 * ------------------------------------------------------------- */

/**
 * @brief Initialize the blinking animation for SET mode
 *
 * @param TargetObject LVGL object to animate
 * @param delay        Animation start delay in milliseconds
 */
static void tempset_Animation(lv_obj_t *TargetObject, int delay)
	{
    ui_anim_user_data_t *user_data =
        lv_malloc(sizeof(ui_anim_user_data_t));

    user_data->target = TargetObject;
    user_data->val = -1;

    lv_anim_init(&tempset_anim);
    lv_anim_set_time(&tempset_anim, 1000);
    lv_anim_set_user_data(&tempset_anim, user_data);
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

/**
 * @brief Initialize heating activity animation
 *
 * Creates a two-phase opacity animation indicating heating ON state.
 *
 * @param TargetObject LVGL object representing heating
 * @param delay        Initial delay in milliseconds
 * @return Always NULL
 */
static lv_anim_t *heat_Animation(lv_obj_t *TargetObject, int delay)
	{
    ui_anim_user_data_t *ud0 = lv_malloc(sizeof(ui_anim_user_data_t));
    ud0->target = TargetObject;
    ud0->val = -1;

    lv_anim_t anim_out;
    lv_anim_init(&anim_out);
    lv_anim_set_time(&anim_out, 3000);
    lv_anim_set_user_data(&anim_out, ud0);
    lv_anim_set_custom_exec_cb(&anim_out, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&anim_out, 255, 0);
    lv_anim_set_path_cb(&anim_out, lv_anim_path_ease_out);
    lv_anim_set_delay(&anim_out, delay);
    lv_anim_set_deleted_cb(&anim_out, _ui_anim_callback_free_user_data);

    ui_anim_user_data_t *ud1 = lv_malloc(sizeof(ui_anim_user_data_t));
    ud1->target = TargetObject;
    ud1->val = -1;

    lv_anim_t anim_in;
    lv_anim_init(&anim_in);
    lv_anim_set_time(&anim_in, 3000);
    lv_anim_set_user_data(&anim_in, ud1);
    lv_anim_set_custom_exec_cb(&anim_in, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&anim_in, 0, 255);
    lv_anim_set_path_cb(&anim_in, lv_anim_path_ease_in);
    lv_anim_set_deleted_cb(&anim_in, _ui_anim_callback_free_user_data);

    timeline_heat = lv_anim_timeline_create();
    lv_anim_timeline_add(timeline_heat, 0, &anim_out);
    lv_anim_timeline_add(timeline_heat, 4000, &anim_in);
    lv_anim_timeline_set_repeat_count(timeline_heat, LV_ANIM_REPEAT_INFINITE);

    return 0;
	}

/* -------------------------------------------------------------
 * Screen construction
 * ------------------------------------------------------------- */

/**
 * @brief Create and initialize the main screen UI
 *
 * This function creates all LVGL objects used by the main screen.
 * It is called only once during the lifetime of the system.
 */
static void draw_main_screen()
	{
	char buf[32];
	
	lv_obj_t * ui_heating = NULL;
	gpio_set_level(LCD_BK_LIGHT, 0);

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
    lv_obj_set_width(ui_dtime, 150); 
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
    sprintf(buf, "%.1f", target_temp - hyst_temp);
    lv_label_set_text(ui_mint, buf);
    lv_obj_set_style_text_color(ui_mint, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_mint, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_mint, &ui_font_segblack20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_maxt = lv_label_create(ui_mainscreen);
    lv_obj_set_width(ui_maxt, 44);
    lv_obj_set_height(ui_maxt, 15);
    lv_obj_set_x(ui_maxt, 268);
    lv_obj_set_y(ui_maxt, 134);
    sprintf(buf, "%.1f", target_temp + hyst_temp);
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

	gpio_set_level(LCD_BK_LIGHT, 1);
	}

/* -------------------------------------------------------------
 * Display update helper
 * ------------------------------------------------------------- */

/**
 * @brief Update temperature-related UI elements
 *
 * @param t         Current temperature
 * @param tmin_val  Minimum hysteresis temperature
 * @param tmax_val  Maximum hysteresis temperature
 * @param animation Animation control selector:
 *                  0 = stop
 *                  1 = start
 *                  2 = update only
 */
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

    int pos = (int)((t - tmin_val) * 100.0f / (tmax_val - tmin_val));
    if (pos < 0) pos = 0;
    if (pos > 100) pos = 100;

    lv_slider_set_value(ui_trange, pos, LV_ANIM_OFF);

    uint32_t color = LOWTXT_COLOR + pos * COLOR_STEP;
    lv_obj_set_style_text_color(ui_acttemp,
                               lv_color_hex(color), 0);

    if (animation == 1)
        ts_anim = lv_anim_start(&tempset_anim);
    else if (animation == 0 && ts_anim)
    {
        lv_anim_pause(ts_anim);
        lv_obj_set_style_opa(ui_acttemp, 255, 0);
    }

    _lock_release(&lvgl_api_lock);
}

/* -------------------------------------------------------------
 * Main screen event loop
 * ------------------------------------------------------------- */

/**
 * @brief Run the main screen UI state machine
 *
 * @param active_screen Reserved for future use
 */
void do_main_screen(int active_screen)
	{
	msg_t msg;
	int therm_state = MEASURING_STATE;
	int nw_connection_state = get_MQTT_connection_state(NULL);
	float t;
	float startget_temp = 0;
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
	set_inactivity = 0; inactivity_set_sec = 0;
	_lock_acquire(&lvgl_api_lock);
	lv_anim_timeline_set_progress(timeline_heat, 32767);
	lv_anim_timeline_pause(timeline_heat);
	_lock_release(&lvgl_api_lock);
	msg.source = MSG_TEMP_DATA;
	xQueueSend(temp_mon_queue, &msg, pdMS_TO_TICKS(20));
	msg.source = MSG_TEMP_DATA;
	xQueueSend(temp_mon_queue, &msg, pdMS_TO_TICKS(20));
	lcd_inactivity = 0;
	while(1)
		{
		if(xQueueReceive(ui_cmd_q, &msg, portMAX_DELAY))
			{
			//ESP_LOGI(TAG, "main screen msg.source = %d", (int)msg.source);
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
				case INACTIVE_SET:
					if(therm_state == SET_STATE)
						{
						get_temp(0, &t);
						display_values(t, target_temp - hyst_temp, target_temp + hyst_temp, 0);
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
						display_values(t, target_temp - hyst_temp, target_temp + hyst_temp, 0);
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
						//ESP_LOGI(TAG, "key pressed 3 times");
						do_option_screen();
						_lock_acquire(&lvgl_api_lock);
						lv_scr_load_anim(ui_mainscreen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 100, 0, false);
						_lock_release(&lvgl_api_lock);
						break;
						}
					if(msg.val == PUSH_TIME_LONG)
						{
						if(therm_state == MEASURING_STATE) // enter set state
							{
							startget_temp = target_temp;
							display_values(startget_temp, startget_temp - hyst_temp, startget_temp + hyst_temp, 1);
							therm_state = SET_STATE;
							// start inactivity set
							set_inactivity = 1;
							}
						else if(therm_state == SET_STATE) // exit set state
							{
							// save target temp set before (if changed)
							if(startget_temp != target_temp)
								set_configuration((int)(startget_temp * 100.), (int)(hyst_temp * 100.), (int)(freeze_temp * 100.), on_off_time, maint_time);
							get_temp(0, &t);
							display_values(t, target_temp - hyst_temp, target_temp + hyst_temp, 0);
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
						
						if(msg.val == K_ROT_LEFT && startget_temp > MIN_TARGET_TEMP)
							{
							startget_temp -= 0.1;
							display_values(startget_temp, startget_temp - hyst_temp, startget_temp + hyst_temp, 2);
		    				}
						if(msg.val == K_ROT_RIGHT && startget_temp < MAX_TARGET_TEMP)
							{
							startget_temp += 0.1;
							display_values(startget_temp, startget_temp - hyst_temp, startget_temp + hyst_temp, 2);
		    				}
						}
					break;
				default:
					break;
				}
			}
		}
	}	
