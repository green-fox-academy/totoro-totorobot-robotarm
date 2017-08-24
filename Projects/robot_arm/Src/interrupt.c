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
	GPIO_Init.Mode = GPIO_MODE_IT_RISING_FALLING;
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

	// Set POWER_ON state
	HAL_GPIO_WritePin(GPIOG, POWER_ON, GPIO_PIN_RESET);

	/* Set Interrupt priority group*/
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);

	/* Set Interrupt priority */
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 1);

	/* Enable  in end_stop_threadterrupt */
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI1_IRQHandler(void)
{

	HAL_GPIO_EXTI_IRQHandler(END_STOP1);
}

void EXTI2_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(END_STOP2);
}

void end_stop_thread(void const * argument)
{
	pin_init();
	while (1) {
		EXTI1_IRQHandler();
		HAL_GPIO_WritePin(GPIOG, POWER_ON, GPIO_PIN_SET);
		osDelay(5000);
		HAL_GPIO_WritePin(GPIOG, POWER_ON, GPIO_PIN_RESET);
		osDelay(5000);
		HAL_GPIO_EXTI_Callback(END_STOP1);
	}
	//EXTI2_IRQHandler();
}
