#ifndef __DATA_DISPLAY_H_
#define __DATA_DISPLAY_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include "robot_arm_conf.h"
#include "servo_control.h"
#include <string.h>

extern uint8_t debug;
uint8_t lcd_data_display_on;
char lcd_data_buff[70];

void lcd_data_display_start(void);
void lcd_data_display_stop(void);
void lcd_data_display_thread(void const * argument);

#endif /* __DATA_DISPLAY_H_ */
