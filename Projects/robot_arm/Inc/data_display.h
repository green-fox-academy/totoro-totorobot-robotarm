#ifndef __DATA_DISPLAY_H_
#define __DATA_DISPLAY_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include "robot_arm_conf.h"
#include <string.h>

void lcd_show_data_thread(void const * argument);

#endif /* __DATA_DISPLAY_H_ */
