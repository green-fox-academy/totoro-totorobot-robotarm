/* Includes ------------------------------------------------------------------*/
#include "servo_control.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void servo_confing(void) {

	// Set servo position to 0 degree

	pwm_conf[0].instance = SERVO0_INST;
	pwm_conf[0].period = SERVO0_PERIOD;
	pwm_conf[0].prescaler = SERVO0_PRESCALER;
	pwm_conf[0].pulse = SERVO0_MIN_PULSE;

	pwm_conf[1].instance = SERVO1_INST;
	pwm_conf[1].period = SERVO1_PERIOD;
	pwm_conf[1].prescaler = SERVO1_PRESCALER;
	pwm_conf[1].pulse = SERVO1_MIN_PULSE;

	pwm_conf[2].instance = SERVO2_INST;
	pwm_conf[2].period = SERVO2_PERIOD;
	pwm_conf[2].prescaler = SERVO2_PRESCALER;
	pwm_conf[2].pulse = SERVO2_MIN_PULSE;

	pwm_conf[3].instance = SERVO3_INST;
	pwm_conf[3].period = SERVO3_PERIOD;
	pwm_conf[3].prescaler = SERVO3_PRESCALER;
	pwm_conf[3].pulse = SERVO3_MIN_PULSE;

	adc_channels[0] = SERVO0_ADC_CHANNEL;
	adc_channels[1] = SERVO1_ADC_CHANNEL;
	adc_channels[2] = SERVO2_ADC_CHANNEL;
	adc_channels[3] = SERVO3_ADC_CHANNEL;

	angle[0].min_angle = SERVO0_MIN_ANGLE;
	angle[0].max_angle = SERVO0_MAX_ANGLE;

	angle[1].min_angle = SERVO1_MIN_ANGLE;
	angle[1].max_angle = SERVO1_MAX_ANGLE;

	angle[2].min_angle = SERVO2_MIN_ANGLE;
	angle[2].max_angle = SERVO2_MAX_ANGLE;

	angle[3].min_angle = SERVO3_MIN_ANGLE;
	angle[3].max_angle = SERVO3_MAX_ANGLE;

	return;
}

void pwm_init(void)
{
	for (int i = 0; i < SERVOS; i++) {
		pwm[i].Instance = pwm_conf[i].instance;
		pwm[i].State = HAL_TIM_STATE_RESET;
		pwm[i].Channel = HAL_TIM_ACTIVE_CHANNEL_1;
		pwm[i].Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		pwm[i].Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		pwm[i].Init.CounterMode = TIM_COUNTERMODE_UP;
		pwm[i].Init.Period = pwm_conf[i].period;
		pwm[i].Init.Prescaler = pwm_conf[i].prescaler;
		HAL_TIM_PWM_Init(&pwm[i]);

		pwm_oc_init[i].OCFastMode = TIM_OCFAST_DISABLE;
		pwm_oc_init[i].OCIdleState = TIM_OCIDLESTATE_RESET;
		pwm_oc_init[i].OCMode = TIM_OCMODE_PWM1;
		pwm_oc_init[i].OCPolarity = TIM_OCPOLARITY_LOW;
		pwm_oc_init[i].Pulse = pwm_conf[i].pulse;
		HAL_TIM_PWM_ConfigChannel(&pwm[i], &pwm_oc_init[i], TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&pwm[i], TIM_CHANNEL_1);
	}
	return;
}

void pwm_set_duty_from_adc(uint8_t servo)
{
	pwm_set_duty(get_degrees());

	return;
}


void pwm_set_duty(uint8_t servo, uint8_t rot_degree)
{
	// Calculate pulse width
	int min_duty = (65536 * MIN_POS_DUTY_CYCLE) / 100;
	int max_duty = (65536 * MAX_POS_DUTY_CYCLE) / 100;

	// Set pulse width
	uint16_t pulse = min_duty + ((max_duty - min_duty) * rot_degree) / 180;
	pwm_oc_init.Pulse = pulse;
	HAL_TIM_PWM_ConfigChannel(&pwm[0], &pwm_oc_init, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&pwm[0], TIM_CHANNEL_1);

	if (debug) {
		char tmp[20];
		sprintf(tmp, "Angle: %2d - Pulse: %5d \n", rot_degree, pulse);
		LCD_UsrLog(tmp);
	}

	return;
}


void adc_init(void)
{
	for (int i = 0; i < SERVOS; i++) {
		adc[i].State = HAL_ADC_STATE_RESET;
		adc[i].Instance = ADC3;
		adc[i].Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
		adc[i].Init.Resolution = ADC_RESOLUTION_12B;
		adc[i].Init.EOCSelection = ADC_EOC_SEQ_CONV;
		adc[i].Init.DMAContinuousRequests = DISABLE;
		adc[i].Init.DataAlign = ADC_DATAALIGN_RIGHT;
		adc[i].Init.ContinuousConvMode = DISABLE;
		adc[i].Init.DiscontinuousConvMode = DISABLE;
		adc[i].Init.ScanConvMode = DISABLE;
		HAL_ADC_Init(&adc[i]);

		adc_ch[i].Channel = adc_channels[i];
		adc_ch[i].Offset = 0;
		adc_ch[i].Rank = 1;
		adc_ch[i].SamplingTime = ADC_SAMPLETIME_480CYCLES;
		HAL_ADC_ConfigChannel(&adc[i], &adc_ch[i]);
	}

	return;
}

uint16_t adc_measure(void)
{
	HAL_ADC_Start(&adc[0]);
	HAL_ADC_PollForConversion(&adc[0], HAL_MAX_DELAY);
	uint16_t value = HAL_ADC_GetValue(&adc[0]);
	HAL_ADC_Stop(&adc[0]);

	if (debug) {
		char tmp[20];
		sprintf(tmp, "ADC value: %d\n", value);
		LCD_UsrLog(tmp);
	}

	return value;
}

/**
  * @brief  Calculates degree of rotation from ADC measurement
  * @param  None
  * @retval Degree (0-180)
  */
uint8_t get_degrees(void)
{
	uint16_t adc_value = adc_measure();
	uint8_t degrees = (uint32_t) ( adc_value * (MAX_DEGREE - MIN_DEGREE)) / (MAX_ADC_VALUE - MIN_ADC_VALUE);

	return degrees;
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
