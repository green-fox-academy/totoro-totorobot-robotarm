#include "interrupt.h"

void pin_init(void)
{
	GPIO_InitTypeDef GPIO_Init;

	// Enable GPIO clocks
	__HAL_RCC_GPIOG_CLK_ENABLE();

	// POWER_ON pin
	// D4 PG7
	GPIO_Init.Pin = GPIO_PIN_7;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &GPIO_Init);

	// Set POWER_ON and MOVEMENT_LED state
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);

	return;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_pin)
{
	switch (GPIO_pin) {

	case GPIO_PIN_1:
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
		buttons[4].state = 1;
		buttons[0].state = 1;
		break;

	case GPIO_PIN_2:
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
		buttons[5].state = 1;
		buttons[0].state = 1;
		break;

	case GPIO_PIN_3:
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
		buttons[6].state = 1;
		buttons[0].state = 1;
		break;
	}
}




