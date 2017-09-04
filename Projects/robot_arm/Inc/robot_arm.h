#ifndef __ROBOT_ARM_H_
#define __ROBOT_ARM_H_

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery_ts.h"
#include "lwip/sockets.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define UDP_SERVER_PORT		54002

typedef struct {
    int32_t x;
    int32_t y;
} coordinate_t;

ADC_HandleTypeDef adc_handle;
ADC_ChannelConfTypeDef adc_ch_conf;
TIM_HandleTypeDef pwm_handle;
TIM_OC_InitTypeDef pwm_oc_init;
uint16_t position; 							// angle of rotation 0-180 degrees
uint8_t debug;
int udp_server_ready;
char *client_ip;
int client_sock;

void sending_packet(void);
void socket_client_thread(void const *argument);
void create_buttons_YGB(void);
void create_button_R(void);
void drawing_stage(char *sys_opening_scr);
void red_button_animation(char *sys_stop);
void yellow_button_animation(char *sys_restart);
void green_button_animation(void);
void blue_button_animation(void);
void circle_delete_animation(coordinate_t last_ts_coord, TS_StateTypeDef ts_state);
void border_limit_settings(TS_StateTypeDef ts_state);
void mouse_coordinate_thread(void const * argument);
void string_splitter(void);
void touch_screen_test_thread(void const * argument);
void udp_server_thread(void const *argument);
void socket_server_thread(void const *argument);


#endif /* __ROBOT_ARM_H_ */
