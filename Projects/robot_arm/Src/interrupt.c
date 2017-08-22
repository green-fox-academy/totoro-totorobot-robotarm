#include "interrupt.h"

#define END_STOP1		GPIO_PIN_6
#define END_STOP2		GPIO_PIN_6
#define POWER_ON		GPIO_PIN_7
#define END_STOP_POWER	GPIO_PIN_7

void end_stop_init(void)
{
	HAL_MspInit();
	HAL_GPIO_WritePin(GPIOC, END_STOP_POWER, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOG, POWER_ON, GPIO_PIN_RESET);

	/* Set Interrupt priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 1);

	/* Enable  Inend_stop_threadterrupt */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void end_stop_thread(void const * argument)
{
/*	end_stop_init();
	HAL_GPIO_EXTI_IRQHandler(END_STOP1);

	EXTI_IRQHandler();*/
}
