/**
  ******************************************************************************
  * @file    stm32f7xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.0.3
  * @date    22-April-2016 
  * @brief   HAL MSP module.
  *          This file template is located in the HAL folder and should be copied 
  *          to the user folder.
  *         
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]


  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/** @addtogroup STM32F7xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspInit(void)
{

}
/**
  * @brief  DeInitializes the Global MSP.
  * @param  None  
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually  
            modified by the user
   */
}
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
		// Enable GPIO clocks
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();

		GPIO_InitTypeDef GPIO_Init;

		GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_Init.Pull = GPIO_NOPULL;

		//END_STOP1
		//2 D1 PC6
		GPIO_Init.Pin = GPIO_PIN_6;
		HAL_GPIO_Init(GPIOC, &GPIO_Init);

		//END_STOP2
		//3 D2 PG6
		GPIO_Init.Pin = GPIO_PIN_6;
		HAL_GPIO_Init(GPIOG, &GPIO_Init);

		// END_STOP_POWER
		//1 D0 PC7
		GPIO_Init.Pin = GPIO_PIN_7	;
		HAL_GPIO_Init(GPIOC, &GPIO_Init);

		// POWER_ON
		//5 D4 PB4
		GPIO_Init.Pin = GPIO_PIN_7	;
		HAL_GPIO_Init(GPIOG, &GPIO_Init);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	// Enable Timer Clocks
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_TIM12_CLK_ENABLE();

	/*
	 *  Set up pins
	 */

	// Enable GPIO clocks
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_Init;

	// Common settings
	GPIO_Init.Speed = GPIO_SPEED_FAST;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Mode = GPIO_MODE_AF_PP;

	// SERVO0
	// 4 D3 PB4 TIM3_CH1
	GPIO_Init.Pin = GPIO_PIN_4;
	GPIO_Init.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOB, &GPIO_Init);

	// SERVO1
	// 7 D6 PH6 TIM12_CH1
	GPIO_Init.Pin = GPIO_PIN_6;
	GPIO_Init.Alternate = GPIO_AF9_TIM12;
	HAL_GPIO_Init(GPIOH, &GPIO_Init);

	// SERVO2
	// 2 D9 PA15 TIM2_CH1
	GPIO_Init.Pin = GPIO_PIN_15;
	GPIO_Init.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &GPIO_Init);

	// SERVO3
	// 3 D10 PA8 TIM1_CH1
	GPIO_Init.Pin = GPIO_PIN_8;
	GPIO_Init.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOA, &GPIO_Init);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	// Enable ADC clock
	__HAL_RCC_ADC3_CLK_ENABLE();

	/*
	 *  Set up pins
	 */

	GPIO_InitTypeDef GPIO_Init;

	// Enable GPIO clocks
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	// Common settings
	GPIO_Init.Speed = GPIO_SPEED_FAST;
	GPIO_Init.Pull = GPIO_NOPULL;
	GPIO_Init.Mode = GPIO_MODE_ANALOG;

	// SERVO0
	// 1 A0 PA0 ADC3_IN0
	GPIO_Init.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_Init);

	// SERVO1
	// 2 A1 PF10 ADC3_IN8
	GPIO_Init.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOF, &GPIO_Init);

	// SERVO2
	// 3 A2 PF9 ADC3_IN7
	GPIO_Init.Pin = GPIO_PIN_9;
	HAL_GPIO_Init(GPIOF, &GPIO_Init);

	// SERVO3
	//4 A3 PF8 ADC3_IN6
	GPIO_Init.Pin = GPIO_PIN_8;
	HAL_GPIO_Init(GPIOF, &GPIO_Init);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
