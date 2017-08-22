#include "interrupt.h"

#define END_STOP1		GPIO_PIN_6
#define END_STOP2		GPIO_PIN_6

void end_stop_init(void)
{
	/* Set Interrupt priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 1);

	/* Enable  Inend_stop_threadterrupt */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void end_stop_thread(void const * argument)
{
	end_stop_init();
	HAL_GPIO_EXTI_IRQHandler(END_STOP1);
	EXTI_IRQHandler(void);
}
