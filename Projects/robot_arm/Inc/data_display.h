#ifndef __DATA_DISPLAY_H_
#define __DATA_DISPLAY_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "sd_card.h"
#include "robot_arm_conf.h"
#include "servo_control.h"
#include <string.h>

#define BUTTON_WIDTH		70
#define BUTTON_2_WIDTH		48
#define BUTTON_HEIGHT		50
#define BUTTON_X_START_VERT	396
#define BUTTON_Y_START_VERT	14
#define BUTTON_DIST_Y		16
#define BUTTON_DIST_X		16

extern uint8_t debug;
uint8_t lcd_data_display_on;
char lcd_data_buff[70];

void start_lcd_data_display(void);
void stop_lcd_data_display(void);
void lcd_data_display_thread(void const * argument);
void draw_buttons(void);

#endif /* __DATA_DISPLAY_H_ */
