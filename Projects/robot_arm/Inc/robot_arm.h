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

int client_sock;
char *client_ip;

//uint8_t debug;

void sending_packet(void);
void socket_client_thread(void const *argument);

void socket_server_thread(void const *argument);
void udp_server_thread(void const *argument);

void create_buttons_YGB(void);
void create_button_R(void);

void drawing_stage(char *sys_opening_scr);
void circle_delete_animation(coordinate_t last_ts_coord, TS_StateTypeDef ts_state);
void border_limit_settings(TS_StateTypeDef ts_state);

void red_button_animation(char *sys_stop);
void yellow_button_animation(char *sys_restart);
void green_button_animation(void);
void blue_button_animation(void);

//void mouse_coordinate_thread(void const *argument);
void the_drawing_function(void);

void string_splitter(void);

void touch_screen_test_thread(void const *argument);

#endif /* __ROBOT_ARM_H_ */
