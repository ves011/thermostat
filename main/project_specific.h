/*
 * project_specific.h
 *
 *  Created on: Jan 3, 2026
 *      Author: viorel_serbu
 */

#ifndef MAIN_PROJECT_SPECIFIC_H_
#define MAIN_PROJECT_SPECIFIC_H_

#define CONFIG_STORE_HISTORY					1
#define CONFIG_CONSOLE_MAX_COMMAND_LINE_LENGTH	1024

#define ACTIVE_CONTROLLER	(THERMOSTAT)
#define TEST_BUILD 			1
#if TEST_BUILD == 1
	#define WITH_CONSOLE
	#define TEST1
	#define CTRL_DEV_ID					(98)
#else
	#define CTRL_DEV_ID					(1)
#endif

#define COMM_PROTO			MQTT_PROTO
//#define OTA_SUPPORT


#define WIFI_STA_ON			(1)
#define WIFI_AP_ON			(0)		//have to be enabled also in sdkconfig


#define WIFI_CONNECTED_BIT  BIT0
#define IP_CONNECTED_BIT    BIT1
#define MQTT_CONNECTED_BIT  BIT2


/* GPIOs */
#define DS18B20_PIN       	(1) //(CONFIG_ONE_WIRE_GPIO)
#define ACT_CMD_PIN			(0) //actuator command pin

//lcd
#define USE_LCD				1
#define LCD_CS				10
#define LCD_RST				7
#define LCD_DC				8
#define LCD_MOSI			6
#define LCD_SCLK			5
#define LCD_BK_LIGHT		20

// rot encoder pins
#define ROT_ENCODER
#define ROT_ENC_S1			(3)
#define ROT_ENC_S2			(4)
#define ROT_ENC_KEY			(21)


/*--------------------------------------------------------*/
#define DEV_NAME			"Room thermostat"
#define MQTT_PUBLISH		1 

#define MSG_TEMP_DATA		(1)
#define MSG_START_MON		(2)
#define MSG_STOP_MON		(3)
#define MSG_ACT_TIME		(4)

#define SET_STATE			2
#define MEASURING_STATE		1
#define ACT_TEMP			2
#define HEATING_ONOFF		5
#define INACTIVE_SET		7


#define SET_INACTIVITY_TIME	10
#define LCD_INACTIVITY_TIME	12

/*
 default cofiguration values
*/
#define DEFAULT_TARGET_TEMP		21			// target temperature
#define DEFAULT_ONOFF_CYCLE		300			// duration of on/off cycle in seconds
#define DEFAULT_HISTEREZIS		0.5			// temperature histerezis
#define DEFAULT_MAINTD			14			// maximum interval between on/off cycles
#define DEFAULT_FREEZET			5			// open actuator below this temp 

/*
	thermostat states
*/
#define OPERATIONAL_STATE				1
#define FREEZING_PROTECTION_STATE		2

#endif /* MAIN_PROJECT_SPECIFIC_H_ */
