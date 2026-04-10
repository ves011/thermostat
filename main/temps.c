/*
 * temps.c
 *
 *  Created on: Jan 8, 2024
 *      Author: viorel_serbu
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "ds18b20_types.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/projdefs.h"
#include "common_defines.h"
//#include "external_defs.h"
#include "hal/gpio_types.h"
#include "mqtt_ctrl.h"
#include "project_specific.h" 
#include "ds18b20.h"
#include "lcd.h"
#include "utils.h"
//#include "state_screen.h"
#include "temps.h"

static int num_temp_devices;
static const char* TAG = "temps op:";
static char *command = "t";
QueueHandle_t temp_mon_queue;

static int temp_trigger_sec;

float	target_temp, hyst_temp, freeze_temp;
int		on_off_time, maint_time;

static int current_state;
static int act_state;
time_t last_act_op, open_maint_ts;

static void get_thermostat_conf();


//esp_timer_handle_t act_timer;		// timer used to measure ACT_OPERATING_TIME interval

struct
	{
    struct arg_str *op;
    struct arg_int *arg;
    struct arg_str *arg1;
    struct arg_end *end;
	} t_args;

static void act_op(int op, float t, bool force);
	
/*
read temperature for maximum NTEMP devices
not needed but i keep it here in case of future extensions
device 0 is the room temperature sensor	
*/	
static void temp_timer_callback(void* arg)
	{
	msg_t msg;
	time_t t = time(NULL);
	if(t % temp_trigger_sec == 0)
		{
		msg.source = MSG_TEMP_DATA;
		xQueueSend(temp_mon_queue, &msg, 0);
		}
	}
/*
static void IRAM_ATTR act_timer_callback(void* arg)
	{
	msg_t msg;
	msg.source = MSG_ACT_TIME;
	xQueueSendFromISR(temp_mon_queue, &msg, NULL);
	}
*/
void tmon(void *pvParams)
	{
	msg_t msg;
	float tc = 0., ts = 0., tp[2] = {-100, -100.};
	char obuf[256], buf[40], mbuf[512];
	time_t time_stamp;
	struct tm timeinfo;
	uint64_t hwad = 0;
	esp_timer_handle_t temp_timer;
	bool monitor_running = false;
	esp_timer_create_args_t temp_timer_args = 
		{
    	.callback = &temp_timer_callback,
        .name = "temp_timer"
    	};
	temp_mon_queue = xQueueCreate(10, sizeof(msg_t));
	if(!temp_mon_queue)
		{
		ESP_LOGE(TAG, "Cannot create temp_mon_queue");
		esp_restart();
		}
	
	ESP_ERROR_CHECK(esp_timer_create(&temp_timer_args, &temp_timer));
	//autostart temperature monitor with 60 sec period
	msg.source = MSG_START_MON;
	msg.val = 60;
	xQueueSend(temp_mon_queue, &msg, pdMS_TO_TICKS(20));
	//-------------------------------------------------
	
	while(1)
		{
		if(xQueueReceive(temp_mon_queue, &msg, portMAX_DELAY))
			{
			//ESP_LOGI(TAG, "msg.source: %d", (int)msg.source);
			if(msg.source == MSG_TEMP_DATA && num_temp_devices)
				{
				time(&time_stamp);
				localtime_r(&time_stamp, &timeinfo);
				strftime(mbuf, sizeof(mbuf), "%Y-%m-%dT%H:%M:%S", &timeinfo);
				strcat(mbuf, "\1");
				
				ESP_ERROR_CHECK(ds18b20_trigger_all_temp_conv());
				ESP_ERROR_CHECK(ds18b20_get_temperature(0, &ts));
				hwad = get_addr(0);
				
				tc = ts;
				msg.val = (int)(tc * 100.);
				msg.source = ACT_TEMP;
				msg.val = (int)(tc * 100.);
				xQueueSend(ui_cmd_q, &msg, pdMS_TO_TICKS(20));
				
				tp[0] = tp[1];
				tp[1] = tc;
				if(tp[0] > -90.) //valid reading
					{
					if(tp[0] < freeze_temp && tp[1] < freeze_temp)
						act_op(1, tc, true);
					else if(tp[0] < target_temp - hyst_temp && tp[1] < target_temp - hyst_temp)
						act_op(1, tc, false);
					else if(tp[0] > target_temp + hyst_temp && tp[1] >  target_temp + hyst_temp)
						act_op(0, tc, false);
					}
				//ESP_LOGI(TAG, "tp0 = %.2f, tp1 = %.2f / %.2f, %.2f", tp[0], tp[1], target_temp, hyst_temp);
 
				sprintf(obuf, "0 - %d %6.3f %6.3f ", act_state, tc, ts);
				sprintf(buf, "%llX\1%d\1%.3f\1%.3f\1", hwad, act_state, tc, ts);
				strcat(mbuf, buf);
				//ESP_LOGI(TAG, "%s", obuf);
				publish_topic(TOPIC_MONITOR, mbuf, 0, 0);

				// check for maintenance time
				if(open_maint_ts == 0)
					{
					if(time_stamp - last_act_op > maint_time * 86400) // check for maintenance time
						{
						act_op(1, tc, false); //open actuator
						open_maint_ts = time_stamp;
						}
					}
				else
					{
					if(time_stamp - open_maint_ts > on_off_time * 3 && tp[0] > freeze_temp && tp[1]  > freeze_temp)
						{
						open_maint_ts = 0;
						act_op(0, tc, false); //open actuator
						}
					}
				}
			else if(msg.source == MSG_START_MON) // start monitoring
				{
				if(!monitor_running)
					{
					temp_trigger_sec = msg.val;
					ESP_LOGI(TAG, "Starting monitor timer");
					ESP_ERROR_CHECK(esp_timer_start_periodic(temp_timer, 1000000));
					monitor_running = true;
					}
				else
					ESP_LOGI(TAG, "Monitor timer already started");
				}
			else if(msg.source == MSG_STOP_MON) // stop monitoring
				{
				if(monitor_running)
					{
					int ret = esp_timer_stop(temp_timer);
					ESP_LOGI(TAG, "Stop monitor timer: %d", ret);
					monitor_running = false;
					}
				else
					ESP_LOGI(TAG, "Monitor timer not started");
				}
			}
		}
	}
void get_temp(int idx, float *temperature)
	{
	ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion(idx));
	ESP_ERROR_CHECK(ds18b20_get_temperature(idx, temperature));
	ESP_LOGI(TAG, "temperature read from DS18B20[%d]: %.2fC", idx, *temperature);
	}
	
uint64_t get_t_hwadd(int idx)
	{
	if(idx < num_temp_devices)
		return get_addr(idx);
	else
 		return 0;
	}

int do_temp(int argc, char **argv)
	{
	msg_t msg;
	if(argc && argv && argv[0] && strcmp(argv[0], command))
		return 0;
	int nerrors = arg_parse(argc, argv, (void **)&t_args);
	if (nerrors != 0)
		{
		//arg_print_errors(stderr, t_args.end, argv[0]);
		return 1;
		}
	if(strcmp(t_args.op->sval[0], "r") == 0)
		{
		float temperature;
		if(t_args.arg->count == 0)
			{
			for(int i = 0; i < num_temp_devices; i++)
				{
				ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion(i));
				ESP_ERROR_CHECK(ds18b20_get_temperature(i, &temperature));
				ESP_LOGI(TAG, "temperature read from DS18B20[%d]: %.2fC", i, temperature);
				}
			}
		else
			{
			ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion(t_args.arg->ival[0]));
			ESP_ERROR_CHECK(ds18b20_get_temperature(t_args.arg->ival[0], &temperature));
			ESP_LOGI(TAG, "temperature read from DS18B20[%d]: %.2fC", t_args.arg->ival[0], temperature);
			}
		}
	else if(strcmp(t_args.op->sval[0], "rc") == 0)
		{
		if(t_args.arg->count > 0)
			{
			int j = 0;
			float t;
			char obuf[256], buf[20];
			
			while(j < t_args.arg->ival[0])
				{
				sprintf(obuf, "%5d - ", j);
				for(int i = 0; i < num_temp_devices; i++)
					{
					ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion(i));
					ESP_ERROR_CHECK(ds18b20_get_temperature(i, &t));
					sprintf(buf, "%6.2f ", t);
					strcat(obuf, buf);
					}
				ESP_LOGI(TAG, "%s", obuf);
				j++;
				} 
			}
		}
	else if(strcmp(t_args.op->sval[0], "s") == 0)
		{
		num_temp_devices = ds18b20_init(DS18B20_RESOLUTION_12B);
		}
	else if(strcmp(t_args.op->sval[0], "start") == 0)
		{
		if(t_args.arg->count && t_args.arg->ival[0] > 0)
			{
			msg.source = MSG_START_MON;
			msg.val = t_args.arg->ival[0];
			xQueueSend(temp_mon_queue, &msg, pdMS_TO_TICKS(20));
			}
		}
	else if(strcmp(t_args.op->sval[0], "stop") == 0)
		{
		msg.source = MSG_STOP_MON;
		xQueueSend(temp_mon_queue, &msg, pdMS_TO_TICKS(20));
		}
	else if(strcmp(t_args.op->sval[0], "state") == 0)
		{
		struct tm timeinfo;
		char mbuf[256], tbuf[40];
		localtime_r(&last_act_op, &timeinfo);
		strftime(tbuf, sizeof(tbuf), "%Y-%m-%dT%H:%M:%S", &timeinfo);
		sprintf(mbuf, "%d\1%llx\1%d\1%s\1%.2f\1%.2f\1%d\1%d\1%.2f\1", 
			num_temp_devices, get_addr(0), act_state, tbuf, target_temp, hyst_temp, on_off_time, maint_time, freeze_temp);
		publish_topic(TOPIC_STATE, mbuf, 0, 0);
		ESP_LOGI(TAG, "num_temp_devices = %d / act_state = %d - %s", num_temp_devices, act_state, tbuf);
		}
	else if(strcmp(t_args.op->sval[0], "act") == 0)
		{
		if(t_args.arg->count)
			{
			act_state = t_args.arg->ival[0];
			act_op(t_args.arg->ival[0], 0.0, true);
			}
		ESP_LOGI(TAG, "Actuator state = %d", act_state);
		}
	// t conf 2100 target
	else if(strcmp(t_args.op->sval[0], "conf") == 0)
		{
		if(t_args.arg->count) // set target value in NVS
			{
			int val = t_args.arg->ival[0];
			if(t_args.arg1->count)
				{
				char keyname[18] = {0};
				if(!strcmp(t_args.arg1->sval[0], TARGET_TEMP))
					strcpy(keyname, TARGET_TEMP);
				else if(!strcmp(t_args.arg1->sval[0], HYSTERESIS))
					strcpy(keyname, HYSTERESIS);
				else if(!strcmp(t_args.arg1->sval[0], FREEZE_TEMP))
					strcpy(keyname, FREEZE_TEMP);
				else if(!strcmp(t_args.arg1->sval[0], ONOFF_CYCLE))
					strcpy(keyname, ONOFF_CYCLE);
				else if(!strcmp(t_args.arg1->sval[0], MAINT_CYCLE))
					strcpy(keyname, MAINT_CYCLE);
				if(strlen(keyname))
					{
					nvs_handle_t handle;
					int err;
					if((err = nvs_open(THERMOSTATNS, NVS_READWRITE, &handle)) == ESP_OK)
						{
						err = nvs_set_i32(handle, keyname, val);
						if(err == ESP_OK)
							{
							if(!strcmp(t_args.arg1->sval[0], TARGET_TEMP))
								target_temp = val / 100.;
							else if(!strcmp(t_args.arg1->sval[0], HYSTERESIS))
								hyst_temp = val / 100.;
							else if(!strcmp(t_args.arg1->sval[0], FREEZE_TEMP))
								freeze_temp = val / 100.;
							else if(!strcmp(t_args.arg1->sval[0], ONOFF_CYCLE))
								on_off_time = val;
							else if(!strcmp(t_args.arg1->sval[0], MAINT_CYCLE))
								maint_time = val;
							}
						else
							ESP_LOGI(TAG, "Error setting %s value: %s / %d", keyname, esp_err_to_name(err), err);

						nvs_commit(handle);
						nvs_close(handle);
						}	
					}
				else
					ESP_LOGI(TAG, "Unknown configuration target: %s", t_args.arg1->sval[0]);
				}	
			}
		else
			{
			ESP_LOGI(TAG, "configuration - target temp (%s) : %.2f", TARGET_TEMP, target_temp);
			ESP_LOGI(TAG, "configuration - hysteresis (%s)  : %.2f", HYSTERESIS, hyst_temp);
			ESP_LOGI(TAG, "configuration - freeze temp (%s) : %.2f", FREEZE_TEMP, freeze_temp);
			ESP_LOGI(TAG, "configuration - on/off cycle (%s): %d", ONOFF_CYCLE, on_off_time);
			ESP_LOGI(TAG, "configuration - on/off cycle (%s): %d", MAINT_CYCLE, maint_time);
			}
		}
	return 0;
	}
int register_temps()
	{
	int ret = 0;
	char mbuf[80];
	gpio_config_t io_conf;
	io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << ACT_CMD_PIN);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(ACT_CMD_PIN, 0);
	num_temp_devices = ds18b20_init(DS18B20_RESOLUTION_12B);
	get_thermostat_conf();
	//get_configuration();
	current_state = OPERATIONAL_STATE;
	act_state = -1;

	if(num_temp_devices > 1)
		{
		sprintf(mbuf, "More temp probes found.\nTemp measuremens will be taken from probe[0]: %llu", get_addr(0));
		publish_topic(TOPIC_ERROR, mbuf, 0, 0);
		}
	else if(num_temp_devices == 0)
		{
		sprintf(mbuf, "No temp probe found");
		publish_topic(TOPIC_ERROR, mbuf, 0, 0);
		ret = ESP_FAIL;
		//do_state_screen();
		}
	else
		{
		ESP_LOGI(TAG, "using 1 temp sensor");
		}

    last_act_op = time(NULL);
    open_maint_ts = 0; 
	t_args.op = arg_str1(NULL, NULL, "<op>", "op: r | s");
	t_args.arg= arg_int0(NULL, NULL, "<arg>", "set sensor #");
	t_args.arg1 = arg_str0(NULL, NULL, "target", "target value");
	t_args.end = arg_end(1);
	const esp_console_cmd_t t_cmd =
		{	
		.command = "t",
		.help = "read (r)|start|stop|conf value target",
		.hint = NULL,
		.func = &do_temp,
		.argtable = &t_args
		};
	ESP_ERROR_CHECK(esp_console_cmd_register(&t_cmd));
	
	if(xTaskCreate(tmon, "tmon_task", 8192, NULL, USER_TASK_PRIORITY, NULL) != pdPASS)
		{
		ESP_LOGI(TAG, "Unable to create temp mon task");
		my_esp_restart();
		}
	return ret;
	}

void act_op(int op, float t, bool force)
	{
	msg_t msg = {0};
	time_t ts = time(NULL);
	if(force || ts - last_act_op >= on_off_time)
		{
		if(op != act_state)
			{
			// last_act_op: timestamp of last actuator state change (ON or OFF)
			last_act_op = ts;
			gpio_set_level(ACT_CMD_PIN, op);
			ESP_LOGI(TAG, "act op %d  %d %.2f", act_state, op, t);
			act_state = op;
			msg.source = HEATING_ONOFF;
			msg.val = op;
			xQueueSend(ui_cmd_q, &msg, pdMS_TO_TICKS(20)); 
			}
		}
	}
static void get_thermostat_conf()
	{
	nvs_handle handle;
	int err;
	int32_t val;

	target_temp = DEFAULT_TARGET_TEMP;
	hyst_temp = DEFAULT_HYSTERESIS;
	freeze_temp = DEFAULT_FREEZET;
	on_off_time = DEFAULT_ONOFF_CYCLE;
	maint_time = DEFAULT_MAINTD;

	if((err = nvs_open(THERMOSTATNS, NVS_READONLY, &handle)) == ESP_OK)
		{
		err = nvs_get_i32(handle, TARGET_TEMP, &val);
		if(err == ESP_OK)
			target_temp = val / 100.;
		else
			ESP_LOGI(TAG, "Error getting %s value. Taking default: %.2f (%s / %d)", TARGET_TEMP, target_temp, esp_err_to_name(err), err);
		err = nvs_get_i32(handle, HYSTERESIS, &val);
		if(err == ESP_OK)
			hyst_temp = val / 100.;
		else
			ESP_LOGI(TAG, "Error getting %s value. Taking default: %.2f (%s / %d)", HYSTERESIS, hyst_temp, esp_err_to_name(err), err);
		err = nvs_get_i32(handle, ONOFF_CYCLE, &val);
		if(err == ESP_OK)
			on_off_time = val;
		else
			ESP_LOGI(TAG, "Error getting %s value. Taking default: %d (%s / %d)", ONOFF_CYCLE, on_off_time, esp_err_to_name(err), err);
		err = nvs_get_i32(handle, FREEZE_TEMP, &val);
		if(err == ESP_OK)
			freeze_temp = val / 100.;
		else
			ESP_LOGI(TAG, "Error getting %s value. Taking default: %.2f (%s / %d)", FREEZE_TEMP, freeze_temp, esp_err_to_name(err), err);
		err = nvs_get_i32(handle, MAINT_CYCLE, &val);
		if(err == ESP_OK)
			maint_time = val;
		else
			ESP_LOGI(TAG, "Error getting %s value. Taking default: %d (%s / %d)", MAINT_CYCLE, maint_time, esp_err_to_name(err), err);
		nvs_close(handle);
		}
	else
		ESP_LOGI(TAG, "Error getting thermostat configuration: %s / %d", esp_err_to_name(err), err);
	}
	
void set_configuration(int targett, int hystt, int freezet, int onoffc, int maintt)
	{
	nvs_handle handle;
	int err;
	if((err = nvs_open(THERMOSTATNS, NVS_READWRITE, &handle)) == ESP_OK)
		{
		err = nvs_set_i32(handle, TARGET_TEMP, targett);
		if(err != ESP_OK)
			ESP_LOGI(TAG, "Error setting %s value: %s / %d", TARGET_TEMP, esp_err_to_name(err), err);
		else
			target_temp = targett / 100.;
		
		err = nvs_set_i32(handle, HYSTERESIS, hystt);
		if(err != ESP_OK)
			ESP_LOGI(TAG, "Error setting %s value: %s / %d", HYSTERESIS, esp_err_to_name(err), err);
		else
			hyst_temp = hystt / 100.;
		
		err = nvs_set_i32(handle, FREEZE_TEMP, freezet);
		if(err != ESP_OK)
			ESP_LOGI(TAG, "Error setting %s value: %s / %d", FREEZE_TEMP, esp_err_to_name(err), err);
		else
		 	freeze_temp = freezet / 100.;
	
		err = nvs_set_i32(handle, ONOFF_CYCLE, onoffc);
		if(err != ESP_OK)
			ESP_LOGI(TAG, "Error setting %s value: %s / %d", ONOFF_CYCLE, esp_err_to_name(err), err);
		else
			on_off_time = onoffc;
		
		err = nvs_set_i32(handle, MAINT_CYCLE, maintt);
		if(err != ESP_OK)
			ESP_LOGI(TAG, "Error setting %s value: %s / %d", MAINT_CYCLE, esp_err_to_name(err), err);
		else
			maint_time = maintt;
					
		nvs_commit(handle);
		nvs_close(handle);
		}
	else
		ESP_LOGI(TAG, "Error setting thermostat configuration: %s / %d", esp_err_to_name(err), err);
	}