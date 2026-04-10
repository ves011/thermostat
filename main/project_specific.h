/**
 * @file project_specific.h
 * @brief Project-wide configuration, build options, and hardware definitions
 *
 * This header centralizes all project-specific configuration used across
 * the firmware. It defines:
 *  - Build-time feature flags (test vs production)
 *  - Active controller type
 *  - Communication protocol selection
 *  - GPIO assignments
 *  - UI and input device configuration
 *  - Thermostat state definitions
 *  - Default configuration values
 *  - Message identifiers used for inter-task communication
 *
 * Design notes:
 *  - This file contains no code, only compile-time configuration.
 *  - All modules include this header to remain consistent.
 *  - Changing values here affects system-wide behavior.
 */

#ifndef MAIN_PROJECT_SPECIFIC_H_
#define MAIN_PROJECT_SPECIFIC_H_

/* -------------------------------------------------------------
 * Console and build configuration
 * ------------------------------------------------------------- */

/** Enable storage of console command history */
#define CONFIG_STORE_HISTORY                    1

/** Maximum allowed console command line length */
#define CONFIG_CONSOLE_MAX_COMMAND_LINE_LENGTH    1024

/**
 * Select active controller type.
 * Expected values are defined in common_defines.h.
 */
#define ACTIVE_CONTROLLER    (THERMOSTAT)

/** Enable test build configuration */
#define TEST_BUILD             0

#if TEST_BUILD == 1
    /** Enable interactive console in test builds */
    #define WITH_CONSOLE
    #define TEST1
    /** Device ID used during test builds */
    #define CTRL_DEV_ID                    (98)
#else
    /** Device ID used in production builds */
    #define CTRL_DEV_ID                    (1)
#endif

/* -------------------------------------------------------------
 * Communication and protocol selection
 * ------------------------------------------------------------- */

/** Communication protocol used by the device (e.g. MQTT) */
#define COMM_PROTO            MQTT_PROTO

/* Uncomment to enable OTA support */
//#define OTA_SUPPORT

/* -------------------------------------------------------------
 * Wi-Fi configuration
 * ------------------------------------------------------------- */

/** Enable Wi-Fi station mode */
#define WIFI_STA_ON            (1)

/**
 * Enable Wi-Fi access point mode.
 * Must also be enabled in sdkconfig if used.
 */
#define WIFI_AP_ON            (0)

/* -------------------------------------------------------------
 * GPIO assignments
 * ------------------------------------------------------------- */

/** One-wire bus GPIO for DS18B20 temperature sensor */
#define DS18B20_PIN           (1)

/** GPIO driving the heating actuator (relay or valve) */
#define ACT_CMD_PIN            (0)

/* -------------------------------------------------------------
 * LCD display configuration
 * ------------------------------------------------------------- */

/** Enable LCD support */
#define USE_LCD                1

/** LCD chip-select GPIO */
#define LCD_CS                10

/** LCD reset GPIO */
#define LCD_RST                7

/** LCD data/command select GPIO */
#define LCD_DC                8

/** LCD SPI MOSI GPIO */
#define LCD_MOSI            6

/** LCD SPI clock GPIO */
#define LCD_SCLK            5

/** LCD backlight control GPIO */
#define LCD_BK_LIGHT        20

/* -------------------------------------------------------------
 * Rotary encoder configuration
 * ------------------------------------------------------------- */

/** Enable rotary encoder support */
#define ROT_ENCODER

/** Rotary encoder signal A GPIO */
#define ROT_ENC_S1            (3)

/** Rotary encoder signal B GPIO */
#define ROT_ENC_S2            (4)

/** Rotary encoder push-button GPIO */
#define ROT_ENC_KEY            (21)

/* -------------------------------------------------------------
 * Device identity and MQTT
 * ------------------------------------------------------------- */

/** Human-readable device name */
#define DEV_NAME            "Room thermostat"

/** Enable MQTT publishing */
#define MQTT_PUBLISH        1 

/* -------------------------------------------------------------
 * Message identifiers (inter-task communication)
 * ------------------------------------------------------------- */

/** Temperature sampling request / data message */
#define MSG_TEMP_DATA        (1)

/** Start temperature monitoring */
#define MSG_START_MON        (2)

/** Stop temperature monitoring */
#define MSG_STOP_MON        (3)

/** Actuator timing message */
#define MSG_ACT_TIME        (4)

/* -------------------------------------------------------------
 * Thermostat UI and control states
 * ------------------------------------------------------------- */

/** UI is in SET (user adjustment) state */
#define SET_STATE            2

/** UI is in normal measuring state */
#define MEASURING_STATE        1

/** Message source for actuator temperature updates */
#define ACT_TEMP            2

/** Message source indicating heating on/off state */
#define HEATING_ONOFF        5

/** Message indicating SET inactivity timeout */
#define INACTIVE_SET        7

/* -------------------------------------------------------------
 * User-configurable limits
 * ------------------------------------------------------------- */

/** Minimum allowed target temperature (°C) */
#define MIN_TARGET_TEMP        10.0

/** Maximum allowed target temperature (°C) */
#define MAX_TARGET_TEMP        28.0

/** Timeout (seconds) before exiting SET state due to inactivity */
#define SET_INACTIVITY_TIME    10

/** Timeout (seconds) before LCD backlight or UI inactivity */
#define LCD_INACTIVITY_TIME    12

/* -------------------------------------------------------------
 * Default thermostat configuration
 * ------------------------------------------------------------- */

/** Default target temperature (°C) */
#define DEFAULT_TARGET_TEMP        21

/* -------------------------------------------------------------
 * Default minimum on/off cycle duration (seconds) 
 * applicable to wax actuators
 * ------------------------------------------------------------- */
#define DEFAULT_ONOFF_CYCLE        300

/** Default temperature hysteresis (°C) */
#define DEFAULT_HYSTERESIS        0.5

/** Default maintenance interval (days) */
#define DEFAULT_MAINTD            14

/** Default freeze protection temperature (°C) */
#define DEFAULT_FREEZET            5

/* -------------------------------------------------------------
 * Thermostat operational states
 * ------------------------------------------------------------- */

/** Normal operating state */
#define OPERATIONAL_STATE                1

/** Freeze protection active state */
#define FREEZING_PROTECTION_STATE        2

#endif /* MAIN_PROJECT_SPECIFIC_H_ */

