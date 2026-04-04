/*
 * lcd.c
 *
 *  Created on: Jan 8, 2026
 *      Author: viorel_serbu
 */

#include <stdio.h>
#include <sys/unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sys/lock.h"
#include "time.h"
#include "lvgl.h"
#include "utils.h"
#include "esp_lcd_panel_st7789.h"
#include "project_specific.h"
#include "common_defines.h"
#include "styles.h"
#include "external_defs.h"
#include "boot_screen.h"
#include "main_screen.h"
#ifdef ROT_ENCODER
	#include "rot_enc.h"
#endif
#include "lcd.h"
static char *TAG = "lcd op";
_lock_t lvgl_api_lock;
uint32_t inactivity_set_sec, set_inactivity, inactivity_lcd_sec, lcd_inactivity;

int init_lcd_completed;

QueueHandle_t ui_cmd_q;

static void ui_task(void *pvParameters);

#define BOOT_SCREEN		1
#define MAIN_SCREEN		2
#define SET_SCREEN		3

static void clock_timer_callback(void *args)
	{
	msg_t msg;
	time_t ts = time(NULL);
	if(ts % 60 == 0)
		{
		msg.source = CLOCK_TICK_1M;
		xQueueSendFromISR(ui_cmd_q, &msg, NULL);
		}
	if(set_inactivity)
		{
		inactivity_set_sec++;
		if(inactivity_set_sec >= SET_INACTIVITY_TIME)
			{
    		msg.source = INACTIVE_SET;
    		msg.val = inactivity_set_sec;
    		xQueueSendFromISR(ui_cmd_q, &msg, NULL);
    		set_inactivity = inactivity_set_sec = 0;
    		}
    	}
	}

static void config_clock_timer() 
	{
	const esp_timer_create_args_t clock_timer_args =
		{
         .callback = &clock_timer_callback,
         .name = ""
		};
	esp_timer_handle_t clock_timer;
    ESP_ERROR_CHECK(esp_timer_create(&clock_timer_args, &clock_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(clock_timer, 1000000));
	}

static void lcd_increase_lvgl_tick(void *arg)
	{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
	}

static void lcd_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
	{
    //lcd_update_callback(disp);
    esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // because SPI LCD is big-endian, we need to swap the RGB bytes order
    lv_draw_sw_rgb565_swap(px_map, (offsetx2 + 1 - offsetx1) * (offsety2 + 1 - offsety1));
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);
	}

static bool lcd_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
	{
    lv_display_t *disp = (lv_display_t *)user_ctx;
    lv_display_flush_ready(disp);
    return false;
	}

static void lcd_init()
	{
	inactivity_set_sec = set_inactivity = inactivity_lcd_sec = lcd_inactivity;
	gpio_config_t bk_gpio_config = 
		{
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LCD_BK_LIGHT
    	};
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    
    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = 
    	{
        .sclk_io_num = LCD_SCLK,
        .mosi_io_num = LCD_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t), 	//ILI9341
        //.max_transfer_sz = 0									//ST7789
    	};
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = 
    	{
        .dc_gpio_num = LCD_DC,
        .cs_gpio_num = LCD_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    	};
    ESP_LOGI(TAG, "Attach the LCD to the SPI bus");
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = 
    	{
        .reset_gpio_num = LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        //.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    	};

    ESP_LOGI(TAG, "Install ST7789 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
	ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
	ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 0, 0));
	ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
	
	ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));
	
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));

    gpio_set_level(LCD_BK_LIGHT, 1);
    
    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    
    lv_display_t *display = lv_display_create(LCD_H_RES, LCD_V_RES);
    size_t draw_buffer_sz = LCD_H_RES * LVGL_DRAW_BUF_LINES * sizeof(lv_color16_t);

    void *buf1 = spi_bus_dma_memory_alloc(LCD_HOST, draw_buffer_sz, 0);
    assert(buf1);
    void *buf2 = spi_bus_dma_memory_alloc(LCD_HOST, draw_buffer_sz, 0);
    assert(buf2);
    
    // initialize LVGL draw buffers
    lv_display_set_buffers(display, buf1, buf2, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_PARTIAL);
    // associate the mipi panel handle to the display
    lv_display_set_user_data(display, panel_handle);
    // set color depth
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    // set the callback which can copy the rendered image to an area of the display
    lv_display_set_flush_cb(display, lcd_flush_cb);
    
    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = 
    	{
        .callback = &lcd_increase_lvgl_tick,
        .name = "lvgl_tick"
    	};
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));
    
    ESP_LOGI(TAG, "Register io panel event callback for LVGL flush ready notification");
    const esp_lcd_panel_io_callbacks_t cbs = {.on_color_trans_done = lcd_flush_ready,};
    /* Register done callback */
    ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, display));
    
    //config_inactivity_timer();
    config_clock_timer();
    init_styles();
	lv_disp_t * dispp = lv_display_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
	
	}

void lvgl_task(void *pvParameters)
	{
	ui_cmd_q = xQueueCreate(10, sizeof(msg_t));
	if(!ui_cmd_q)
		{
		ESP_LOGE(TAG, "Unable to create UI cmd queue");
		my_esp_restart();
		}
	uint32_t time_threshold_ms = 1000 / CONFIG_FREERTOS_HZ, time_till_next = 0;
	lcd_init();
	if(xTaskCreate(ui_task, "ui_task", 8192, NULL, 5, NULL) != pdPASS)
		{
		ESP_LOGE(TAG, "Unable to start UI task");
		esp_restart();
		}
	#ifdef ROT_ENCODER
		init_rotenc(ui_cmd_q);
	#endif
	init_lcd_completed = 1;
	while(1) 
		{
		_lock_acquire(&lvgl_api_lock);
  		time_till_next = lv_timer_handler();
  		_lock_release(&lvgl_api_lock);
		if(time_till_next == LV_NO_TIMER_READY) 
  			time_till_next = time_threshold_ms;
		else
			time_till_next = time_till_next > time_threshold_ms ? time_till_next : time_threshold_ms;
        usleep(1000 * time_till_next);
		}
	}
	
static void ui_task(void *pvParameters)
	{
	while(1)
		{
		if(!controller_op_registered)
			do_boot_screen();
		vTaskDelay(10);
		do_main_screen(0);
		}
	}
	


