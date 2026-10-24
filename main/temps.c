/*
 * temps.c
 *
 *  Created on: Jan 8, 2024
 *      Author: viorel_serbu
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "cmd_system.h"
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
#include "hal/gpio_types.h"
#include "mqtt_ctrl.h"
#include "project_specific.h" 
#include "utils.h"
#include "ds18b20.h"
#include "lcd.h"
#include "ota.h"
#include "temps.h"

static int num_temp_devices;
static const char* TAG = "temps op:";
char *command = "temp";
QueueHandle_t temp_mon_queue;

static int temp_trigger_sec;

float	target_temp, hyst_temp, freeze_temp, dt_pub;
int		on_off_time, maint_time, corr_temp;

static int current_state;
static int act_state;
time_t last_act_op, open_maint_ts;
static float last_temp_rep = 0.;

static void get_thermostat_conf();
static void publish_state();

static struct
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

void tmon(void *pvParams)
	{
	msg_t msg;
	float ts = 0., tp[2] = {-100, -100.};
	//char obuf[256], buf[40];
	char mbuf[512];
	time_t time_stamp;
	struct tm timeinfo;
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
	publish_state();
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
				
				ESP_ERROR_CHECK(ds18b20_trigger_all_temp_conv());
				ESP_ERROR_CHECK(ds18b20_get_temperature(0, &ts));
				
				ts = ts - ((float)corr_temp) / 100.;
				msg.source = ACT_TEMP;
				msg.val = (int)(ts * 100.);
				xQueueSend(ui_cmd_q, &msg, pdMS_TO_TICKS(20));
				//ESP_LOGI(TAG, "temp mon 0: %d, t = %.2f / %.2f", act_state, ts, last_temp_rep);
				tp[0] = tp[1];
				tp[1] = ts;
				if(tp[0] > -90.) //valid reading
					{
					if(tp[0] < freeze_temp && tp[1] < freeze_temp)
						act_op(1, ts, true);
					else if(tp[0] < target_temp - hyst_temp && tp[1] < target_temp - hyst_temp)
						act_op(1, ts, false);
					else if(tp[0] > target_temp + hyst_temp && tp[1] >  target_temp + hyst_temp)
						act_op(0, ts, false);
					if(fabs(ts - last_temp_rep) > dt_pub)
						{
						char obuf[64];
						ESP_LOGI(TAG, "temp mon: %d, t = %.2f", act_state, ts);
						sprintf(obuf, "\1%s\1%d\1%.2f\1%.2f\1", TEMP_MON, act_state, ts, target_temp);
						strcat(mbuf, obuf);
						publish_topic(TOPIC_MONITOR, mbuf, 0, 0);
						last_temp_rep = ts;
						}
					}

				// check for maintenance time
				if(open_maint_ts == 0)
					{
					if(time_stamp - last_act_op > maint_time * 86400) // check for maintenance time
						{
						act_op(1, ts, false); //open actuator
						open_maint_ts = time_stamp;
						}
					}
				else
					{
					if(time_stamp - open_maint_ts > on_off_time * 3 && tp[0] > freeze_temp && tp[1]  > freeze_temp)
						{
						open_maint_ts = 0;
						act_op(0, ts, false); //open actuator
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
			else if(msg.source == MSG_OTA_START) // start ota update
				{
				char url[64], pname[32];
				strncpy(pname, (char *)msg.vts.m_val, sizeof(msg.vts.m_val));
				strncpy(url, (char *)msg.ifvals.ival, sizeof(msg.ifvals.ival));
				url[sizeof(msg.ifvals.ival) - 1] = 0;
				pname[sizeof(msg.vts.m_val) - 1] = 0;
				ota_task(url, pname);
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
		return 1;

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
		publish_state();
		}
	else if(strcmp(t_args.op->sval[0], "act") == 0)
		{
		if(t_args.arg->count)
			{
			act_op(t_args.arg->ival[0], 0.0, true);
			act_state = t_args.arg->ival[0];
			}
		ESP_LOGI(TAG, "Actuator state = %d", act_state);
		}
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
				else if(!strcmp(t_args.arg1->sval[0], CORR_TEMP))
					strcpy(keyname, CORR_TEMP);
				else if(!strcmp(t_args.arg1->sval[0], DT_PUB))
					strcpy(keyname, DT_PUB);
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
							else if(!strcmp(t_args.arg1->sval[0], DT_PUB))
								dt_pub = val / 100.;
							else if(!strcmp(t_args.arg1->sval[0], ONOFF_CYCLE))
								on_off_time = val;
							else if(!strcmp(t_args.arg1->sval[0], MAINT_CYCLE))
								maint_time = val;
							else if(!strcmp(t_args.arg1->sval[0], CORR_TEMP))
								corr_temp = val;
							nvs_commit(handle);
							publish_state();
							}
						else
							ESP_LOGI(TAG, "Error setting %s value: %s / %d", keyname, esp_err_to_name(err), err);
						nvs_close(handle);
						}	
					}
				else
					ESP_LOGI(TAG, "Unknown configuration target: %s", t_args.arg1->sval[0]);
				}	
			}
		else
			{
			ESP_LOGI(TAG, "configuration - target temp (%s)    : %.2f", TARGET_TEMP, target_temp);
			ESP_LOGI(TAG, "configuration - hysteresis (%s)     : %.2f", HYSTERESIS, hyst_temp);
			ESP_LOGI(TAG, "configuration - freeze temp (%s)    : %.2f", FREEZE_TEMP, freeze_temp);
			ESP_LOGI(TAG, "configuration - dt-pub (%s)         : %.2f", DT_PUB, dt_pub);
			ESP_LOGI(TAG, "configuration - on/off cycle (%s)   : %d", ONOFF_CYCLE, on_off_time);
			ESP_LOGI(TAG, "configuration - on/off cycle (%s)   : %d", MAINT_CYCLE, maint_time);
			ESP_LOGI(TAG, "configuration - correction temp (%s): %d", CORR_TEMP, corr_temp);
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
	act_state = 0;
	num_temp_devices = ds18b20_init(DS18B20_RESOLUTION_12B);
	get_thermostat_conf();
	current_state = OPERATIONAL_STATE;

	if(num_temp_devices > 1)
		{
		sprintf(mbuf, "More temp probes found.\nTemp measurements will be taken from probe[0]: %llu", get_addr(0));
		publish_topic(TOPIC_ERROR, mbuf, 0, 0);
		}
	else if(num_temp_devices == 0)
		{
		sprintf(mbuf, "No temp probe found");
		publish_topic(TOPIC_ERROR, mbuf, 0, 0);
		ret = ESP_FAIL;
		}
	else
		{
		ESP_LOGI(TAG, "using 1 temp sensor");
		}

    last_act_op = time(NULL);
    open_maint_ts = 0; 
	t_args.op = arg_str1(NULL, NULL, "<op>", "op: r | start | stop | act | state| conf");
	t_args.arg= arg_int0(NULL, NULL, "<arg>", "value");
	t_args.arg1 = arg_str0(NULL, NULL, "target", "target");
	t_args.end = arg_end(1);
	const esp_console_cmd_t t_cmd =
		{	
		.command = "temp",
		.help = "read (r)|start|stop|conf value target",
		.hint = NULL,
		.func = &do_temp,
		.argtable = &t_args
		};
	ESP_ERROR_CHECK(esp_console_cmd_register(&t_cmd));
	
	if(xTaskCreate(tmon, "tmon_task", 8192, NULL, USER_TASK_PRIORITY, NULL) != pdPASS)
		RESTART("Unable to create temp mon task");

	return ret;
	}
int do_ota_tmon_task(int argc, char **argv)
	{
	msg_t msg;
	if(argc == 3 && strlen(argv[1]) > 0 && strlen(argv[2]) > 0)
		{
		strcpy((char *)msg.vts.m_val, argv[2]);
		strcpy((char *)msg.ifvals.ival, argv[1]);
		msg.source = MSG_OTA_START;
		xQueueSend(temp_mon_queue, &msg, pdMS_TO_TICKS(20));
		}
	else
		ESP_LOGI(TAG, "%s arguments error", argv[0]);
	return 0;
	}
void act_op(int op, float t, bool force)
	{
	msg_t msg = {0};
	time_t ts = time(NULL);
	char mbuf[256] = {0}, pl[32];
	struct tm timeinfo;
	if(force || ts - last_act_op >= on_off_time)
		{
		if(op != act_state)
			{
			// last_act_op: timestamp of last actuator state change (ON or OFF)
			last_act_op = ts;
			gpio_set_level(ACT_CMD_PIN, op);
			ESP_LOGI(TAG, "act op %d  %d %.2f", act_state, op, t);
			msg.source = HEATING_ONOFF;
			msg.val = op;
			xQueueSend(ui_cmd_q, &msg, pdMS_TO_TICKS(20));
			
			//publish new state
			localtime_r(&ts, &timeinfo);
			strftime(mbuf, sizeof(mbuf), "%Y-%m-%dT%H:%M:%S", &timeinfo);
			sprintf(pl, "\1%s\1%d\1%.2f\1%.2f\1", ACT_MON, op, t, target_temp);
			strcat(mbuf, pl);
			publish_topic(TOPIC_MONITOR, mbuf, 0, 0);
			act_state = op;
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
	dt_pub = DEFAULT_DT_PUB;
	freeze_temp = DEFAULT_FREEZET;
	on_off_time = DEFAULT_ONOFF_CYCLE;
	maint_time = DEFAULT_MAINTD;
	corr_temp = DEFAULT_CORRECTION;

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
		err = nvs_get_i32(handle, CORR_TEMP, &val);
		if(err == ESP_OK)
			corr_temp = val;
		else
			ESP_LOGI(TAG, "Error getting %s value. Taking default: %df (%s / %d)", CORR_TEMP, corr_temp, esp_err_to_name(err), err);
		
		err = nvs_get_i32(handle, DT_PUB, &val);
		if(err == ESP_OK)
			dt_pub = val / 100.;
		else
			ESP_LOGI(TAG, "Error getting %s value. Taking default: %.2f (%s / %d)", DT_PUB, freeze_temp, esp_err_to_name(err), err);
		nvs_close(handle);
		}
	else
		ESP_LOGI(TAG, "Error getting thermostat configuration: %s / %d", esp_err_to_name(err), err);
	}
	
void set_configuration(int targett)
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
		nvs_commit(handle);
		nvs_close(handle);

		publish_state();
		}
	else
		ESP_LOGI(TAG, "Error setting thermostat configuration: %s / %d", esp_err_to_name(err), err);
	}
void publish_state()
	{
	char mbuf[256], obuf[32];
	time_t time_stamp;
	struct tm timeinfo;
	float ts;
	ESP_ERROR_CHECK(ds18b20_trigger_all_temp_conv());
	ESP_ERROR_CHECK(ds18b20_get_temperature(0, &ts));
	ts = ts - ((float)corr_temp) / 100.;
	time(&time_stamp);
	localtime_r(&time_stamp, &timeinfo);
	strftime(mbuf, sizeof(mbuf), "%Y-%m-%dT%H:%M:%S", &timeinfo);
	sprintf(obuf, "\1%s\1%d\1%.2f\1%.2f\1%.2f\1%.2f\1", TEMP_MON, act_state, ts, last_temp_rep, target_temp, hyst_temp);
	strcat(mbuf, obuf);
	publish_topic(TOPIC_STATE, mbuf, 0, 0);
	ESP_LOGI(TAG, "act_state = %d / temp = %.2f previous temp = %.2f", act_state, ts, last_temp_rep);
	}
	
