#ifndef __DATA_DISPLAY_H_
#define __DATA_DISPLAY_H_

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery_ts.h"
#include "cmsis_os.h"
#include "sd_card.h"
#include "robot_arm_conf.h"
#include "servo_control.h"
#include <string.h>

#define BUTTONS		10

typedef struct {
	uint16_t x;
	uint16_t y;
	uint8_t width;
	uint8_t height;
	uint32_t btn_color0;
	uint32_t btn_color1;
	uint32_t text_color0;
	uint32_t text_color1;
	uint16_t text_x0;
	uint16_t text_x1;
	uint16_t text_y;
	char text0[10];
	char text1[10];
	uint8_t touchable;
	uint8_t state;
} button_t;

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

#endif /* __DATA_DISPLAY_H_ */
