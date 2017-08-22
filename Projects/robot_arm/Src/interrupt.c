#include "interrupt.h"

void arm_init(void)
{
	/* Set Interrupt priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 1);

	/* Enable  Interrupt */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void arm_thread(void const * argument)
{
	arm_init();
}
