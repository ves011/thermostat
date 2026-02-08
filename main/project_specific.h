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
#define TEST_BUILD (1)
#if(TEST_BUILD == 1)
	#define WITH_CONSOLE
	#define TEST1
	#define CTRL_DEV_ID					(98)
	#define LOG_SERVER_DEV				"proxy.gnet"
	#define LOG_PORT_DEV				8083
#else
	#define CTRL_DEV_ID					(1)
#endif

#define COMM_PROTO			MQTT_PROTO
#define OTA_SUPPORT


#define WIFI_STA_ON			(1)
#define WIFI_AP_ON			(0)

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
#define ROT_ENC_S1			(4)
#define ROT_ENC_S2			(3)
#define ROT_ENC_KEY			(21)


/*--------------------------------------------------------*/
#define DEV_NAME			"Room thermostat"
#define PD_USER				"thermlcd"				//USER_MQTT = PD_USER|CTRL_DEV_ID
#define MQTT_PUBLISH		1 

/*
Message definitions for device monitor queue
*/
#define MSG_WIFI			1	// wifi connect (.val = 1)/disconnect (.val = 0) event 
#define	MQTT_CONNECT_STATE	100	//mqtt server connection status (.val = 1)/disconnect (.val = 0)
#define MSG_BAT				2	// battery level .val = ADC battery measurement * 1000
#define MSG_LED_FLASH		3	// nw state and remote state flashing
#define NW_STATE_CHANGE		4	// nw connected (.val = 1) / disconnected (.val = 0)
#define REMOTE_STATE_CHANGE	5	// remote connected (.val = 1) / disconnected (.val = 0)
#define INIT_COMPLETE		6	// init completed

#define MSG_TEMP_DATA		(1)
#define MSG_START_MON		(2)
#define MSG_STOP_MON		(3)
#define MSG_ACT_TIME		(4)


#define ACT_TEMP			2
#define NW_STATE			1
#define TMIN_VAL			3
#define TMAX_VAL			4
#define HEATING_ONOFF		5
#define TEMP_SET			6
#define INACTIVE_SET		7
#define INACTIVE_LCD		8

#define SET_INACTIVITY_TIME	10
#define LCD_INACTIVITY_TIME	12

#define CONFIG_FILE				"config.txt"
#define TARGET_TXT				"target temp(deg C): "
#define HISTREZIS_TXT			"target temp histerezis(deg C): "
#define ONOFF_TXT				"ON/OFF cycle duration(sec): "
#define MAINTENANCE_TXT			"maximum interval between actuator ON/OFF cycles (days): "
#define FREEZE_TXT				"freezing protect circuit temp(degC): "
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

/*
	actuator states
*/
#define ACT_CLOSED_STATE				0 // closed and open state must be 0 and 1 because 
#define ACT_OPEN_STATE					1 // the state should match pin level
#define ACT_OP_INPROGRESS				0x10 // value to OR with steady states

#define ACT_OP_STATE					"act_op"
#define GET_STATE						"get_state" 
#define ERROR_MORE_SENSORS				1
#define ERROR_NO_SENSORS				2
#define ERROR_HW_ADDR					3

#endif /* MAIN_PROJECT_SPECIFIC_H_ */
