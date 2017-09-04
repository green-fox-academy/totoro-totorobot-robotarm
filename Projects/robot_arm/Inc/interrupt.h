#ifndef __INTERRUPT_H_
#define __INTERRUPT_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32746g_discovery.h"
#include "stm32f7xx_it.h"
#include "robot_arm_conf.h"

void pin_init(void);
void EXTI1_IRQHandler_Config(void);
void EXTI2_IRQHandler_Config(void);
void EXTI3_IRQHandler_Config(void);
void EXTI3_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI1_IRQHandler(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void end_stop_thread(void const * argument);

extern button_t buttons[BUTTONS];

#endif /* __INTERRUPT_H_ */
