/**
 * @file main.c
 * @brief Application entry point and system initialization sequence
 *
 * This module contains the main entry point (`app_main`) for the firmware.
 * It is responsible for:
 *  - Initializing platform services (NVS, SPIFFS, GPIO, Wi-Fi, MQTT)
 *  - Starting the LVGL UI task
 *  - Executing the system boot sequence
 *  - Sending boot progress notifications to the UI
 *  - Registering console and system commands
 *  - Starting thermostat control and monitoring logic
 *
 * Design notes:
 *  - Boot progress is communicated to the UI through ui_cmd_q using BOOT_MSG.
 *  - System initialization is strictly sequential.
 *  - Failure at any critical step results in a controlled reboot.
 */
 
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <esp_spiffs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include "esp_log.h"
#include "esp_console.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "ntp_sync.h"
#include "common_defines.h"
#include "external_defs.h"
#include "cmd_wifi.h"
#include "cmd_system.h"
#include "tcp_log.h"
#include "utils.h"
#include "mqtt_ctrl.h"
#include "lcd.h"
#include "temps.h"

/**
 * @brief Indicates that a system restart has been requested or is in progress
 *
 * Used to prevent re-entrant restart logic and for diagnostic purposes.
 */
int restart_in_progress;

/**
 * @brief Indicates that controller operations have been fully initialized
 *
 * This flag is set after thermostat control, temperature monitoring,
 * and related system services are registered and ready.
 */
int controller_op_registered;

/** Console prompt prefix */
#define PROMPT_STR	"thermo"

/** Logging tag for main application */
static char *TAG = "thermo";

/**
 * @brief Initialize the Non-Volatile Storage (NVS) subsystem
 *
 * This function initializes the NVS flash storage. If the flash
 * contains no free pages or an incompatible version is detected,
 * the storage is erased and re-initialized.
 *
 * This operation is performed early during system boot.
 */
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
	
/**
 * @brief Main application entry point
 *
 * This function is called by the ESP-IDF runtime after system startup.
 * It performs the complete system initialization sequence in the
 * following order:
 *
 * 1. Set timezone and prepare GPIO interrupt services
 * 2. Start the LVGL UI task and wait for display initialization
 * 3. Notify UI of boot progress at each major step
 * 4. Initialize SPIFFS and NVS storage
 * 5. Load stored configuration and certificates
 * 6. Initialize Wi-Fi and attempt STA connection
 * 7. Start MQTT client and logging subsystem
 * 8. Register console, system, and Wi-Fi commands
 * 9. Synchronize system time via NTP
 * 10. Register thermostat control and temperature monitoring
 * 11. Signal completion (or error) to the UI
 *
 * Any unrecoverable failure during initialization results in
 * a controlled system restart.
 */
void app_main(void)
	{
	msg_t msg;
	int count;
	int temps_probe = ESP_FAIL;
	msg.source = BOOT_MSG;
    dev_conf.cs = CONSOLE_OFF;
    
    /* Set local timezone (used by UI and logging) */
	setenv("TZ","EET-2EEST,M3.4.0/03,M10.4.0/04",1);

	/* Install GPIO ISR service */
	gpio_install_isr_service(0);
	
/* ---------------------------------------------------------
     * Start LVGL UI task
     * --------------------------------------------------------- */
	TaskHandle_t lvgl_task_handle;
	init_lcd_completed = 0;

	xTaskCreate(lvgl_task, "lvgl_task", 8192, NULL, 2, &lvgl_task_handle);
	if(!lvgl_task_handle)
		{
		ESP_LOGE(TAG, "Unable to start lvgl task");
		my_esp_restart();
		}
/* Wait for LVGL initialization to complete */
	count = 0;
	while(init_lcd_completed == 0)
		{
		count++;
		if(count >= 10)
			{
			ESP_LOGE(TAG, "lcd init not completed in 5sec");
			my_esp_restart();
			}
		vTaskDelay(pdMS_TO_TICKS(500));
		}

    /* ---------------------------------------------------------
     * Boot phase notifications and system initialization
     * --------------------------------------------------------- */

    msg.val = BOOT_LCD_READY;
    xQueueSend(ui_cmd_q, &msg, 0);
	spiffs_storage_check();
	initialize_nvs();
	get_nvs_conf();
	get_all_nvscerts();
	msg.val = BOOT_NVS_READY;
    xQueueSend(ui_cmd_q, &msg, 0);
    
/* Wi-Fi initialization and connection */
    initialise_wifi();
	if(!wifi_join(NULL, NULL, JOIN_TIMEOUT_MS))
		ESP_LOGI(TAG, "Failed to connect to %s", dev_conf.sta_ssid);
	else
		ESP_LOGI(TAG, "Connected to %s", dev_conf.sta_ssid);
	
	msg.val = BOOT_NET_READY;
    xQueueSend(ui_cmd_q, &msg, 0);
    
   /* MQTT and logging services */
	mqtt_start();
	tcp_log_init(dev_conf.cs);
	esp_log_set_vprintf(my_log_vprintf);

	
	msg.val = BOOT_MQTT_READY;
    xQueueSend(ui_cmd_q, &msg, 0);

/* Console and command registration */
	esp_console_register_help_command();
	register_system();
	register_wifi();
	
/* Time synchronization */
	sync_NTP_time();
	msg.val = BOOT_CMDS_READY;
    xQueueSend(ui_cmd_q, &msg, 0);
    
 /* Thermostat and temperature monitoring */
	temps_probe = register_temps();
	msg.val = BOOT_TEMP_READY;
    xQueueSend(ui_cmd_q, &msg, 0);
	controller_op_registered = 1;

#ifdef WITH_CONSOLE
	/* ---------------------------------------------------------
     * Optional interactive console (test / debug builds)
     * --------------------------------------------------------- */
	esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
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
#else
	#error Unsupported console type
#endif
	ESP_ERROR_CHECK(esp_console_start_repl(repl));
#endif /* WITH_CONSOLE */
	/* ---------------------------------------------------------
     * Final boot status notification
     * --------------------------------------------------------- */
	if(temps_probe == ESP_FAIL)    
		msg.val = BOOT_ERR_NO_TEMP;
	else
		msg.val = BOOT_DONE;
    xQueueSend(ui_cmd_q, &msg, 0);
	}
