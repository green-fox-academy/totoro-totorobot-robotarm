#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32746g_discovery.h"

void arm_init(void);
void arm_thread(void const * argument);

#endif /* __INTERRUPT_H_ */
