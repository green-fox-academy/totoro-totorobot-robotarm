#include "interrupt.h"

#define END_STOP1		GPIO_PIN_6
#define END_STOP2		GPIO_PIN_6
#define POWER_ON		GPIO_PIN_7

GPIO_InitTypeDef GPIO_Init;

void pin_init(void)
{
	// Enable GPIO clocks
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	//END_STOP1
	//2 D1 PC6
	GPIO_Init.Pin = GPIO_PIN_6;
	GPIO_Init.Mode = GPIO_MODE_IT_FALLING;
	GPIO_Init.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOC, &GPIO_Init);

	//END_STOP2
	//3 D2 PG6
	GPIO_Init.Pin = GPIO_PIN_6;
	GPIO_Init.Mode = GPIO_MODE_IT_FALLING;
	GPIO_Init.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOG, &GPIO_Init);
	// POWER_ON
	//5 D4 PB4
	GPIO_Init.Pin = GPIO_PIN_7	;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOG, &GPIO_Init);

	// Set state
	HAL_GPIO_WritePin(GPIOG, POWER_ON, GPIO_PIN_RESET);

	/* Set Interrupt priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 1);

	/* Enable  in end_stop_threadterrupt */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler(void)
{
	 HAL_GPIO_EXTI_IRQHandler(POWER_ON);
}

void end_stop_thread(void const * argument)
{
	pin_init();
	EXTI1_IRQHandler();
	EXTI2_IRQHandler();
	/*while (1) {
		HAL_GPIO_WritePin(GPIOC, END_STOP_POWER, GPIO_PIN_SET);
		osDelay(1000);
		HAL_GPIO_WritePin(GPIOC, END_STOP_POWER, GPIO_PIN_RESET);
		osDelay(1000);
	}*/

	//HAL_GPIO_EXTI_IRQHandler(END_STOP1);

	//EXTI_IRQHandler();
}
