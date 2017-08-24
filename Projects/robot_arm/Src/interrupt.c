#include "interrupt.h"

#define END_STOP1		GPIO_PIN_6
#define END_STOP2		GPIO_PIN_6
#define POWER_ON		GPIO_PIN_7

GPIO_InitTypeDef GPIO_Init;

void EXTI3_IRQHandler_Config(void)
{
	// Enable GPIO clocks
	__HAL_RCC_GPIOI_CLK_ENABLE();

	//END_STOP1
	//8 D7 PI3
	GPIO_Init.Pin = GPIO_PIN_3;
	GPIO_Init.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_Init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOI, &GPIO_Init);

	/* Set Interrupt priority */
	HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 1);

	/* Enable  in end_stop_threadterrupt */
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

void EXTI2_IRQHandler_Config(void)
{
	// Enable GPIO clocks
	__HAL_RCC_GPIOI_CLK_ENABLE();

	//END_STOP2
	//1 D8 PI2
	GPIO_Init.Pin = GPIO_PIN_2;
	GPIO_Init.Mode = GPIO_MODE_IT_FALLING;
	GPIO_Init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOI, &GPIO_Init);

	/* Set Interrupt priority */
	HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 1);

	/* Enable  in end_stop_threadterrupt */
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}

void pin_init(void)
{
	// Enable GPIO clocks
	__HAL_RCC_GPIOG_CLK_ENABLE();

	// POWER_ON
	//5 D4 PG7
	GPIO_Init.Pin = GPIO_PIN_7	;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &GPIO_Init);

	// Set POWER_ON state
	HAL_GPIO_WritePin(GPIOG, POWER_ON, GPIO_PIN_RESET);
}

void EXTI3_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI2_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if ((GPIO_Pin == GPIO_PIN_3) || (GPIO_Pin == GPIO_PIN_2)) {
		HAL_GPIO_WritePin(GPIOG, POWER_ON, GPIO_PIN_SET);
	}
}

void end_stop_thread(void const * argument)
{
	pin_init();
	while (1) {
		HAL_GPIO_EXTI_Callback();
	}
	//EXTI2_IRQHandler();
}
