/*
 * temps.h
 *
 *  Created on: Jan 8, 2024
 *      Author: viorel_serbu
 */

#ifndef MAIN_TEMPS_H_
#define MAIN_TEMPS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/*
 NVS configuration: namespace name and keys name
*/
#define THERMOSTATNS		"thermostat"
#define TARGET_TEMP			"target-temp"		
#define ONOFF_CYCLE			"on/off-cycle"
#define FREEZE_TEMP			"freeze-temp"
#define HYSTERESIS     		"hysteresis"
#define MAINT_CYCLE   		"maint-cycle"
 

/*
 default configuration values
*/
#define DEFAULT_TARGET_TEMP		21			// target temperature
#define DEFAULT_ONOFF_CYCLE		300			// duration of on/off cycle in seconds
#define DEFAULT_HYSTERESIS		0.5			// temperature histerezis
#define DEFAULT_MAINT			14			// maximum interval between on/off cycles
#define DEFAULT_FREEZET			5			// open actuator below this temp 


#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)
#define DS_SAMPLE_PERIOD     (1000)   // milliseconds


extern float	target_temp, hyst_temp, freeze_temp;
extern int		on_off_time, maint_time;
extern QueueHandle_t temp_mon_queue;;

int register_temps();
int do_temp(int argc, char **argv);
void get_temp(int idx, float *temperature);
uint64_t get_t_hwadd(int idx);
void set_configuration(int targett, int histt, int freezet, int onoffc, int maintt);

#endif /* MAIN_TEMPS_H_ */
