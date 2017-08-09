#ifndef __ROBOT_ARM_H_
#define __ROBOT_ARM_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include "uart.h"
#include "servo_control.h"

uint8_t demo_on;
coord_cart_t points[10];

void start_demo(void);
void stop_demo(void);
void demo_thread(void const * argument);

#endif /* __ROBOT_ARM_H_ */
