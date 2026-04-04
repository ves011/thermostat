/*
 * lcd.h
 *
 *  Created on: Jan 8, 2026
 *      Author: viorel_serbu
 */

#ifndef LCD_LCD_H_
#define LCD_LCD_H_

#define LCD_HOST  SPI2_HOST

// The pixel number in horizontal and vertical
#define LCD_H_RES              320
#define LCD_V_RES              240
#define LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
//#define LCD_PIXEL_CLOCK_HZ     (40 * 1000 * 1000)

#define LVGL_DRAW_BUF_LINES    20 // number of display lines in each draw buffer
#define LVGL_TICK_PERIOD_MS    2

// Bit number used to represent command and parameter
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8

extern QueueHandle_t ui_cmd_q;
extern uint32_t inactivity_set_sec, set_inactivity, inactivity_lcd_sec, lcd_inactivity;
extern _lock_t lvgl_api_lock;
extern int init_lcd_completed;

void lvgl_task(void *pvParameters);

#endif /* LCD_LCD_H_ */
