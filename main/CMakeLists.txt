# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

idf_component_register(
					   SRCS "main.c"
							"temps.c"
							"config.c"
							"../lcd/lcd.c"

							"../lcd/styles.c"
							"../lcd/boot_screen.c"
							"../lcd/main_screen.c"
							"../lcd/ui.c"
							"../lcd/option_screen.c"
							"../lcd/state_screen.c"
							"../lcd/ui_font_hemihead60.c"
							"../lcd/ui_font_segblack20.c"
							"../lcd/ui_font_segblack16.c"
							"../lcd/images/ui_img_wifi_png.c"			#wifi
							"../lcd/images/ui_img_frame_00_png.c"		#heating
							"../lcd/images/ui_img_wifi_error_png.c"		#wifi-error
							#"../lcd/spi_lcd_touch_example_main.c"
							#"../lcd/lvgl_demo_ui.c"
							"../../esp32_common/tcp/ntp_sync.c"
							"../../esp32_common/utils/utils.c"
							"../../esp32_common/cmds/cmd_wifi.c"
							"../../esp32_common/cmds/cmd_system.c"
							"../../esp32_common/tcp/mqtt_ctrl.c"
							"../../esp32_common/tcp/tcp_log.c"
							"../../esp32_common/owb/onewire_bus_api.c"
							"../../esp32_common/owb/onewire_bus_impl_rmt.c"
							"../../esp32_common/owb/onewire_crc.c"
							"../../esp32_common/owb/onewire_device.c"
							"../../esp32_common/owb/ds18b20.c"
							"../../esp32_common/utils/ota.c"
							"../../esp32_common/utils/rot_enc.c"
                    INCLUDE_DIRS 
                    		"."
                    		"../certs"
                    		"../lcd"
							"../../esp32_common"
                    		"../../esp32_common/tcp"
							"../../esp32_common/utils"
							"../../esp32_common/cmds"
							"../../esp32_common/tcp"
							"../../esp32_common/owb"
						)
#idf_build_set_property(COMPILE_OPTIONS "-DLV_CONF_INCLUDE_SIMPLE=1" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-DLV_CONF_PATH=\"../lcd/lv_conf.h\"" APPEND)
#idf_build_set_property(COMPILE_OPTIONS "-I../lcd" APPEND)