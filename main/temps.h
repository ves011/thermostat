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
#include <stdint.h>
#define THERMOSTATNS		"thermostat"
#define TARGET_TEMP			"target-temp"		
#define ONOFF_CYCLE			"on/off-cycle"
#define FREEZE_TEMP			"freeze-temp"
#define HYSTERESIS     		"hysteresis"
#define MAINT_CYCLE   		"maint-cycle"
#define CORR_TEMP			"corr-temp"
#define DT_PUB				"dt-pub"
 

/*
 default configuration values
*/
#define DEFAULT_TARGET_TEMP		21			// target temperature
#define DEFAULT_DT_PUB			0.3			// delta temp to publish
#define DEFAULT_ONOFF_CYCLE		300			// duration of on/off cycle in seconds
#define DEFAULT_HYSTERESIS		0.5			// temperature histerezis
#define DEFAULT_MAINT			14			// maximum interval between on/off cycles
#define DEFAULT_FREEZET			5			// open actuator below this temp 
#define DEFAULT_CORRECTION		0			// temp correction factor in 0.01 decC increments 


#define DS18B20_RESOLUTION   (DS18B20_RESOLUTION_12_BIT)
#define DS_SAMPLE_PERIOD     (1000)   // milliseconds

#define TEMP_MON			"t"
#define ACT_MON				"a"

/*
 * TOPIC_MONITOR format
 * field1 - date/time
 * field2 - message type
 *	"t" - temperature change
 *  "a" - actuator state change
 * field3 - actuator state
 * field4 - temp value
*/

extern float	target_temp, hyst_temp, freeze_temp, dt_pub;
extern int		on_off_time, maint_time, corr_temp;
extern QueueHandle_t temp_mon_queue;;
extern char *command;

int register_temps();
int do_temp(int argc, char **argv);
void get_temp(int idx, float *temperature);
uint64_t get_t_hwadd(int idx);
void set_configuration(int targett);//, int hystt, int freezet, int onoffc, int maintt, int corrtemp, int dtpub);

#endif /* MAIN_TEMPS_H_ */
