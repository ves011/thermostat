/*
 * config.c
 *
 *  Created on: Jan 4, 2026
 *      Author: viorel_serbu
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "sys/stat.h"
#include "esp_log.h"
#include "project_specific.h"
#include "common_defines.h"
#include "config.h"

struct
	{
    struct arg_str *op;		// read | write
    struct arg_str *arg;	// which value t | h | c | f | m
    struct arg_int *value;	// value to r/w - if target, hist or freeze = (int)(value * 100)
    struct arg_end *end;
	} c_args;
	
static const char *TAG = "config";

extern float	target_temp, hist_temp, freeze_temp;
extern int		on_off_time, maint_time;


/*
config.txt structure
	line 1 --> target temperature (degC)
	line 2 --> target histerezis (degC)
	line 3 --> freeze temp (degC)
	line 4 --> actuator on/off cycle duration (sec)
	line 5 --> on/off maintenance actuator interval (days) 
*/

void get_configuration()
	{
	FILE *f;
	char buf[80];
	struct stat st;
	bool tt = false, ht = false, onoff = false, mt = false, ft = false;
	target_temp = DEFAULT_TARGET_TEMP;
	hist_temp = DEFAULT_HISTEREZIS;
	freeze_temp = DEFAULT_FREEZET;
	on_off_time = DEFAULT_ONOFF_CYCLE;
	maint_time = DEFAULT_MAINTD;
	freeze_temp = DEFAULT_FREEZET;
	if (stat(BASE_PATH"/"CONFIG_FILE, &st) != 0)
		ESP_LOGI(TAG, "no %s file found. Taking default values", CONFIG_FILE);
	else
		{
		f = fopen(BASE_PATH"/"CONFIG_FILE, "r");
		if(f)
			{
			target_temp = DEFAULT_TARGET_TEMP;
			hist_temp = DEFAULT_HISTEREZIS;
			on_off_time = DEFAULT_ONOFF_CYCLE;
			maint_time = DEFAULT_MAINTD;
			freeze_temp = DEFAULT_FREEZET;
			while(fgets(buf, 64, f))
				{
				if(strstr(buf, TARGET_TXT))
					{
					target_temp = atof(buf + strlen(TARGET_TXT));
					tt = true;
					}
				if(strstr(buf, HISTREZIS_TXT))
					{
					hist_temp = atof(buf + strlen(HISTREZIS_TXT));
					ht = true;
					}
				if(strstr(buf, FREEZE_TXT))
					{
					freeze_temp = atoi(buf + strlen(FREEZE_TXT));
					ft = true;
					}
				if(strstr(buf, ONOFF_TXT))
					{
					on_off_time = atoi(buf + strlen(ONOFF_TXT));
					onoff = true;
					}
				if(strstr(buf, MAINTENANCE_TXT))
					{
					maint_time = atoi(buf + strlen(MAINTENANCE_TXT));
					mt = true;
					}
				}
			fclose(f);
			if(!tt)
				ESP_LOGI(TAG, "no value for target temp found. taking default value: %.2f", target_temp);
			if(!ht)
				ESP_LOGI(TAG, "no value for target temp histerezis found. taking default value: %.2f", hist_temp);
			if(!ft)
				ESP_LOGI(TAG, "no value for freezing temp protection found. taking default value: %.2f", freeze_temp);
			if(!onoff)
				ESP_LOGI(TAG, "no value for on/off cycle duration found. taking default value: %d", on_off_time);
			if(!mt)
				ESP_LOGI(TAG, "no value for mainenance time found. taking default value: %d", maint_time);
			}
		else
			ESP_LOGI(TAG, "Error opening configuration file. Taking default values");
		}
	}
	
void set_configuration(int targett, int histt, int freezet, int onoffc, int maintt)
	{
	char buf[80];
	int setconf = 0;
	FILE *f = fopen(BASE_PATH"/"CONFIG_FILE, "w");
	if (f == NULL)
		{
		ESP_LOGE(TAG, "Failed to create configuration file");
		}
	else 
		{
		sprintf(buf, "%s%.2f\n", TARGET_TXT, (float)(targett) / 100.);
		if(fputs(buf, f) >= 0)
			{
			sprintf(buf, "%s%.2f\n", HISTREZIS_TXT, (float)(histt) / 100.);
			if(fputs(buf, f) >= 0)
				{
				sprintf(buf, "%s%.2f\n", FREEZE_TXT, (float)(freezet) / 100.);
				if(fputs(buf, f) >= 0)
					{
					sprintf(buf, "%s%d\n", ONOFF_TXT, onoffc);
					if(fputs(buf, f) >= 0)
						{
						sprintf(buf, "%s%d\n", MAINTENANCE_TXT, maintt);
						if(fputs(buf, f) >= 0)
							{
							setconf = 1;
							}
						}
					}
				}
			}
		fclose(f);
		if(!setconf)
			{
			unlink(BASE_PATH"/"CONFIG_FILE);
			ESP_LOGE(TAG, "Error while saving parameters. Config file deleted to avoid corruption");
			ESP_LOGE(TAG, "No parameters updated");
			}
		else
			{
			target_temp = (float)targett / 100.; hist_temp = (float)histt / 100.; freeze_temp = (float)freezet / 100.;
			on_off_time = onoffc; maint_time = maintt; 
			}
		}
	}

int do_conf(int argc, char **argv)
	{
	int wr = 0;
	int tt, ht, ft, ot, mt;
	int nerrors = arg_parse(argc, argv, (void **)&c_args);
	if (nerrors != 0)
		{
		arg_print_errors(stderr, c_args.end, argv[0]);
		return ESP_FAIL;
		}
	if(strcmp(c_args.op->sval[0], "r") == 0)
		{
		get_configuration();
		ESP_LOGI(TAG, "%s %.2f", TARGET_TXT, target_temp);
		ESP_LOGI(TAG, "%s %.2f", HISTREZIS_TXT, hist_temp);
		ESP_LOGI(TAG, "%s %.2f", FREEZE_TXT, freeze_temp);
		ESP_LOGI(TAG, "%s %d", ONOFF_TXT, on_off_time);
		ESP_LOGI(TAG, "%s %d", MAINTENANCE_TXT, maint_time);
		}
	if(strcmp(c_args.op->sval[0], "w") == 0)
		{
		tt = target_temp * 100.; ht = hist_temp * 100.; ft = freeze_temp * 100.;
		ot = on_off_time; mt = maint_time;
		if(c_args.arg->count && !strcmp(c_args.arg->sval[0], "t") && c_args.value->count)
			{
			wr = 1;
			tt = c_args.value->ival[0];
			}
		if(c_args.arg->count && !strcmp(c_args.arg->sval[0], "h") && c_args.value->count)
			{
			wr = 1;
			ht = c_args.value->ival[0];
			}
		if(c_args.arg->count && !strcmp(c_args.arg->sval[0], "f") && c_args.value->count)
			{
			wr = 1;
			ft = c_args.value->ival[0];
			}
		if(c_args.arg->count && !strcmp(c_args.arg->sval[0], "c") && c_args.value->count)
			{
			wr = 1;
			ot = c_args.value->ival[0];
			}
		if(c_args.arg->count && !strcmp(c_args.arg->sval[0], "m") && c_args.value->count)
			{
			wr = 1;
			mt = c_args.value->ival[0];
			}
		if(wr)
			set_configuration(tt, ht, ft, ot, mt);
		}
	return 0;	
	}

void register_config()
	{
	c_args.op = arg_str1(NULL, NULL, "<op>", "op: r | w");
	c_args.arg= arg_str0(NULL, NULL, "<arg>", "target value: <t|h|c|f|m>");
	c_args.value= arg_int0(NULL, NULL, "<arg>", "value to r/w");
	c_args.end = arg_end(1);
	const esp_console_cmd_t c_cmd =
		{	
		.command = "tconf",
		.help = "read(r) / write (w) configuration value",
		.hint = NULL,
		.func = &do_conf,
		.argtable = &c_args
		};
	ESP_ERROR_CHECK(esp_console_cmd_register(&c_cmd));
	
	}



