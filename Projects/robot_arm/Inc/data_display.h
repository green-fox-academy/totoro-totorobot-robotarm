#ifndef __DATA_DISPLAY_H_
#define __DATA_DISPLAY_H_

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery_ts.h"
#include "cmsis_os.h"
#include "sd_card.h"
#include "robot_arm_conf.h"
#include "servo_control.h"
#include <string.h>

typedef struct {
	uint16_t x;
	uint16_t y;
} touch_t;

TS_StateTypeDef TS_State;
button_t buttons[BUTTONS];
extern uint8_t debug;
uint8_t lcd_data_display_on;
char lcd_data_buff[70];

void start_lcd_data_display(void);
void stop_lcd_data_display(void);
void lcd_data_display_thread(void const * argument);
void draw_buttons(void);
void init_buttons(void);

extern void udp_client_thread(void const *argument);
extern void socket_server_thread(void const *argument);

#endif /* __DATA_DISPLAY_H_ */
