#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <esp_spiffs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_heap_caps.h"
#include "ntp_sync.h"
#include "project_specific.h"
#include "common_defines.h"
#include "external_defs.h"
#include "cmd_wifi.h"
#include "cmd_system.h"
#include "tcp_log.h"
#include "utils.h"
#include "mqtt_ctrl.h"
#include "lcd.h"
#include "temps.h"
#include "config.h"

int restart_in_progress;
int controller_op_registered;

#define PROMPT_STR	"thermo"

static char *TAG = "thermo";

//const char *nvs_cl_crt;
//size_t nvs_cl_crt_sz;

void esp_heap_trace_alloc_hook(void* ptr, size_t size, uint32_t caps)
	{
  	//printf("\nheap alloc ptr: %x / size: %d / caps %x", (unsigned int)ptr, size, (unsigned int)caps);
	}
void esp_heap_trace_free_hook(void* ptr)
	{
  	//printf("\nheap free ptr: %x ", (unsigned int)ptr);
	}

static void initialize_nvs(void)
	{
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
		{
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
		}
	ESP_ERROR_CHECK(err);
	}
	

void app_main(void)
	{
	msg_t msg;
	int temps_probe = ESP_FAIL;
	msg.source = BOOT_MSG;
    dev_conf.cs = CONSOLE_OFF;
	setenv("TZ","EET-2EEST,M3.4.0/03,M10.4.0/04",1);

	gpio_install_isr_service(0);
	
	TaskHandle_t lvgl_task_handle;
	init_lcd_completed = 0;

	xTaskCreate(lvgl_task, "lvgl_task", 8192, NULL, 2, &lvgl_task_handle);
	if(!lvgl_task_handle)
		{
		ESP_LOGE(TAG, "Unable to start lvgl task");
		esp_restart();
		}
	while(init_lcd_completed == 0)
		usleep(10000);

	msg.val = 0;
    //xQueueSend(ui_cmd_q, &msg, 0);
	spiffs_storage_check();
	initialize_nvs();
	my_printf("1 min heap size: %u", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
	get_nvs_conf();
	my_printf("2 min heap size: %u", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
	get_all_nvscerts();
	my_printf("3 min heap size: %u", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
	msg.val = 1;
    xQueueSend(ui_cmd_q, &msg, 0);
    register_wifi();
    my_printf("4 min heap size: %u", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
	if(!wifi_join(NULL, NULL, JOIN_TIMEOUT_MS))
		{
		ESP_LOGI(TAG, "Failed to connect to %s", dev_conf.sta_ssid);
		}
	else
		{
		ESP_LOGI(TAG, "Connected to %s", dev_conf.sta_ssid);
		}
	
	//tcp_log_evt_queue = NULL;
	tcp_log_init();
	my_printf("5 min heap size: %u", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
	esp_log_set_vprintf(my_log_vprintf);
	msg.val = 2;
    xQueueSend(ui_cmd_q, &msg, 0);
	//sync_NTP_time();
	if(mqtt_start() != ESP_OK)
		esp_restart();
	my_printf("6 min heap size: %u", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
	msg.val = 3;
    xQueueSend(ui_cmd_q, &msg, 0);

	esp_console_register_help_command();
	my_printf("7 min heap size: %u", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
	register_system();
	my_printf("8 min heap size: %u", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
	sync_NTP_time();
	msg.val = 4;
    xQueueSend(ui_cmd_q, &msg, 0);
	temps_probe = register_temps();
	my_printf("9 min heap size: %u", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
	register_config();
	my_printf("10 min heap size: %u", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));
	msg.val = 5;
    xQueueSend(ui_cmd_q, &msg, 0);
	controller_op_registered = 1;

#ifdef WITH_CONSOLE
	esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = CONFIG_CONSOLE_MAX_COMMAND_LINE_LENGTH;
#if CONFIG_STORE_HISTORY
	repl_config.history_save_path = BASE_PATH HISTORY_FILE;
#endif

#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    repl_config.task_stack_size = 4096;
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));
    //ESP_LOGI(TAG, "console stack: %d", repl_config.task_stack_size);
#else
	#error Unsupported console type
#endif
	ESP_ERROR_CHECK(esp_console_start_repl(repl));
#endif
	if(temps_probe == ESP_FAIL)    
		msg.val = 101;
	else
		msg.val = 8;
    xQueueSend(ui_cmd_q, &msg, 0);
	}
