/**
 * @file lcd.h
 * @brief LVGL-based LCD user interface public interface
 *
 * This header defines the public interface of the LCD/UI subsystem.
 * It provides:
 *  - LCD hardware and LVGL configuration constants
 *  - Boot progress and error codes used by the UI
 *  - Inter-task communication primitives
 *  - Synchronization objects for LVGL access
 *  - The main LVGL task entry point
 *
 * Design rules:
 *  - All LVGL API calls must be protected by lvgl_api_lock.
 *  - UI logic is event-driven and consumes messages from ui_cmd_q.
 *  - The LCD/UI layer must not perform control or actuator logic.
 *  - Screen modules (boot, main, option, state) are modal and blocking.
 */

#ifndef LCD_LCD_H_
#define LCD_LCD_H_

#include <freertos/idf_additions.h>

/* -------------------------------------------------------------
 * LCD hardware and LVGL configuration
 * ------------------------------------------------------------- */

/**
 * @brief SPI peripheral used for LCD communication
 */
#define LCD_HOST  SPI2_HOST

/** LCD horizontal resolution (pixels) */
#define LCD_H_RES              320

/** LCD vertical resolution (pixels) */
#define LCD_V_RES              240

/**
 * @brief LCD pixel clock frequency (Hz)
 *
 * This value affects SPI bandwidth and display refresh rate.
 */
#define LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
/* Optional higher clock (disabled) */
/* #define LCD_PIXEL_CLOCK_HZ  (40 * 1000 * 1000) */

/**
 * @brief Number of display lines per LVGL draw buffer
 *
 * Smaller values reduce RAM usage at the expense of refresh speed.
 */
#define LVGL_DRAW_BUF_LINES    20

/**
 * @brief LVGL tick period in milliseconds
 *
 * Used by the LVGL tick timer to advance internal timers.
 */
#define LVGL_TICK_PERIOD_MS    2

/* -------------------------------------------------------------
 * LCD command protocol configuration
 * ------------------------------------------------------------- */

/** Number of bits used to encode LCD commands */
#define LCD_CMD_BITS           8

/** Number of bits used to encode LCD command parameters */
#define LCD_PARAM_BITS         8

/* -------------------------------------------------------------
 * Boot progress and error codes
 * ------------------------------------------------------------- */

/**
 * @brief Boot progress and error codes sent to the UI
 *
 * These values are transmitted via ui_cmd_q with source BOOT_MSG
 * during the system initialization sequence.
 *
 * The boot screen displays each stage sequentially until either
 * BOOT_DONE or a BOOT_ERR_* condition is received.
 */
enum {
    /** LCD and LVGL initialization completed */
    BOOT_LCD_READY = 0,

    /** SPIFFS and NVS initialization completed */
    BOOT_NVS_READY = 1,

    /** Network stack initialized and Wi-Fi connected */
    BOOT_NET_READY = 2,

    /** MQTT client started successfully */
    BOOT_MQTT_READY = 3,

    /** Console and system commands registered */
    BOOT_CMDS_READY = 4,

    /** Temperature monitoring subsystem registered */
    BOOT_TEMP_READY = 5,

    /**
     * Boot sequence completed successfully.
     * Causes boot screen to exit and main screen to load.
     */
    BOOT_DONE = 100,

    /**
     * Boot error: no temperature sensor detected.
     * Causes transition to the state screen.
     */
    BOOT_ERR_NO_TEMP = 101
};

/* -------------------------------------------------------------
 * UI inter-task communication and synchronization
 * ------------------------------------------------------------- */

/**
 * @brief UI command queue handle
 *
 * All UI-related events (input, state changes, notifications)
 * are sent through this queue and handled by the active screen.
 */
extern QueueHandle_t ui_cmd_q;

/**
 * @brief SET-mode inactivity elapsed time counter (seconds)
 */
extern uint32_t inactivity_set_sec;

/**
 * @brief Flag indicating SET-mode inactivity monitoring is active
 */
extern uint32_t set_inactivity;

/**
 * @brief LCD inactivity elapsed time counter (seconds)
 */
extern uint32_t inactivity_lcd_sec;

/**
 * @brief LCD inactivity state flag
 *
 * Used to control backlight behavior or UI blanking.
 */
extern uint32_t lcd_inactivity;

/**
 * @brief LVGL API synchronization lock
 *
 * All LVGL API calls must be protected by this lock.
 * Fine-grained locking is used to avoid blocking the render loop.
 */
extern _lock_t lvgl_api_lock;

/**
 * @brief LCD initialization completion flag
 *
 * Set to non-zero once LCD hardware and LVGL initialization
 * are complete and the UI can start receiving messages.
 */
extern int init_lcd_completed;

/* -------------------------------------------------------------
 * LVGL task entry point
 * ------------------------------------------------------------- */

/**
 * @brief LVGL UI task entry point
 *
 * This FreeRTOS task is responsible for:
 *  - Initializing LCD hardware and LVGL
 *  - Running the LVGL render/timer loop
 *  - Dispatching UI messages to screen handlers
 *
 * @param pvParameters Task parameters (unused)
 */
void lvgl_task(void *pvParameters);

#endif /* LCD_LCD_H_ */
