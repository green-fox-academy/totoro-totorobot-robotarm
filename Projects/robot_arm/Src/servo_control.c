#include "servo_control.h"

void servo_config(void)
{
	// Configure up servo related data

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

	servo_pos_conf[0].min_angle = SERVO0_MIN_ANGLE;
	servo_pos_conf[0].max_angle = SERVO0_MAX_ANGLE;

	servo_pos_conf[1].min_angle = SERVO1_MIN_ANGLE;
	servo_pos_conf[1].max_angle = SERVO1_MAX_ANGLE;

	servo_pos_conf[2].min_angle = SERVO2_MIN_ANGLE;
	servo_pos_conf[2].max_angle = SERVO2_MAX_ANGLE;

	servo_pos_conf[3].min_angle = SERVO3_MIN_ANGLE;
	servo_pos_conf[3].max_angle = SERVO3_MAX_ANGLE;

	for (int i = 0; i < SERVOS; i++) {
		servo_pos_conf[i].adc_to_angle_const = (servo_pos_conf[i].max_angle - servo_pos_conf[i].min_angle) / (MAX_ADC_VALUE - MIN_ADC_VALUE);
	}

	for (int i = 0; i < SERVOS; i++) {
		servo_pos_conf[i].angle_to_pulse = (servo_pos_conf[i].max_pulse - servo_pos_conf[i].min_pulse) / (servo_pos_conf[i].max_angle - servo_pos_conf[i].min_angle);
	}

	for (int i = 0; i < SERVOS; i++) {
		servo_pos_conf[i].adc_to_pulse = (servo_pos_conf[i].max_pulse - servo_pos_conf[i].min_pulse) / (MAX_ADC_VALUE - MIN_ADC_VALUE);
	}

	// Set arm to origo
	for (int i = 0; i < SERVOS; i++) {
		servo_pos[i].angle = servo_pos_conf[i].min_angle;
		servo_pos[i].pulse = pwm_conf[i].pulse;
	}

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

		if (debug) {
			sprintf(lcd_log, "Servo%d init done\n", i);
			LCD_UsrLog(lcd_log);
		}


		pwm_oc_init[i].OCFastMode = TIM_OCFAST_DISABLE;
		pwm_oc_init[i].OCIdleState = TIM_OCIDLESTATE_RESET;
		pwm_oc_init[i].OCMode = TIM_OCMODE_PWM1;
		pwm_oc_init[i].OCPolarity = TIM_OCPOLARITY_HIGH;
		pwm_oc_init[i].Pulse = pwm_conf[i].pulse;
		HAL_TIM_PWM_ConfigChannel(&pwm[i], &pwm_oc_init[i], TIM_CHANNEL_1);


		if (debug) {

			sprintf(lcd_log, "Servo%d config channel done\n", i);
			LCD_UsrLog(lcd_log);
		}


		HAL_TIM_PWM_Start(&pwm[i], TIM_CHANNEL_1);

		if (debug) {

			sprintf(lcd_log, "Servo%d started\n", i);
			LCD_UsrLog(lcd_log);
		}
	}
	return;
}

void pwm_set_pulse(uint8_t servo, uint32_t pulse)
{
	// Set pulse width
	pwm_oc_init[servo].Pulse = pulse;

	HAL_TIM_PWM_ConfigChannel(&pwm[servo], &pwm_oc_init[servo], TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&pwm[servo], TIM_CHANNEL_1);

	if (debug) {
		sprintf(lcd_log, "Servo%d pulse: %5d\n", servo, pulse);
		LCD_UsrLog(lcd_log);
	}

	return;
}

void adc_init(void)
{
	// Zero out adc_values array
	memset(adc_values, 0, sizeof(adc_values));

	// General init
	adc.State = HAL_ADC_STATE_RESET;
	adc.Instance = ADC3;
	adc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	adc.Init.Resolution = ADC_RESOLUTION_12B;
	adc.Init.EOCSelection = DISABLE;
	adc.Init.DMAContinuousRequests = ENABLE;
	adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc.Init.ContinuousConvMode = ENABLE;
	adc.Init.DiscontinuousConvMode = DISABLE;
	adc.Init.ScanConvMode = ENABLE;
	adc.Init.NbrOfConversion = 4;
	HAL_ADC_Init(&adc);

	// Init channels
	for (int i = 0; i < SERVOS; i++) {
		adc_ch[i].Channel = adc_channels[i];
		adc_ch[i].Offset = 0;
		adc_ch[i].Rank = i + 1;
		adc_ch[i].SamplingTime = ADC_SAMPLETIME_480CYCLES;
		HAL_ADC_ConfigChannel(&adc, &adc_ch[i]);
	}

	// Start continuous conversion with DMA
	HAL_ADC_Start_DMA(&adc, (uint32_t*) &adc_values, sizeof(adc_values));

	return;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
  /* Turn LED1 on: Transfer process is correct */
  //BSP_LED_On(LED1);
  sprintf(lcd_log, "ADC0: %4d  ADC1: %4d  ADC2: %4d  ADC3: %4d\n", adc_values[0], adc_values[1], adc_values[2], adc_values[3]);
  LCD_UsrLog(lcd_log);

}

void adc_deinit(void)
{
	HAL_ADC_Stop_DMA(&adc);
	return;
}

uint16_t adc_measure(uint8_t servo)
{
	uint16_t adc_value = 0;
	HAL_ADC_Start(&adc);
	HAL_ADC_PollForConversion(&adc, HAL_MAX_DELAY);
	adc_value = HAL_ADC_GetValue(&adc);
	HAL_ADC_Stop(&adc);

	if (debug) {
		// sprintf(lcd_log, "ADC%d value: %d\n", servo, adc_value);
		// LCD_UsrLog(lcd_log);
	}

	return adc_value;
}

uint8_t adc_to_angle(uint8_t servo, uint16_t adc_value)
{
	uint8_t degrees = (float) adc_value * servo_pos_conf[servo].adc_to_angle_const;

	return degrees;
}

uint32_t angle_to_pulse(uint8_t servo, uint8_t degree)
{

	// Calculate pulse width
	uint32_t pulse = (float) degree * servo_pos_conf[servo].angle_to_pulse;

	return pulse;
}

uint32_t adc_to_pulse(uint8_t servo, uint16_t adc_value)
{

	// Calculate pulse width
	uint32_t pulse = (float) adc_value * servo_pos_conf[servo].adc_to_pulse;

	return pulse;
}

void start_adc(void)
{
	adc_init();
	adc_on = 1;

	if (debug) {
		LCD_UsrLog((char*) "ADC started\n");
	}

	return;
}

void stop_adc(void)
{
	adc_deinit();
	adc_on = 0;

	if (debug) {
		LCD_UsrLog((char*) "ADC terminated\n");
	}

	return;
}

void pwm_thread(void const * argument)
{
	pwm_ready = 0;

	if (debug) {
		LCD_UsrLog((char*) "PWM thread started\n");
	}

	// Initialize all PWM channels
	pwm_init();
	pwm_ready = 1;

	if (debug) {
		LCD_UsrLog((char*) "PWM ready\n");
	}

	// Set servo positions
	while (1) {
		for (int i = 0; i < SERVOS; i++) {

			// TODO : check if ADC is running. If so, convert ADC values to pulse herer
			// if (adc_on) { .... }

			// Lock mutex
			osMutexWait(servo_pos_mutex, osWaitForever);

			// Get pulse value
			uint32_t servo_pulse = servo_pos[i].pulse;

			// Release mutex
			osMutexRelease(servo_pos_mutex);

			// Set position
			pwm_set_pulse(i, servo_pulse);

			osDelay(1000);
		}
	}

    while (1) {
        // Terminate thread
        if (debug) {
        	LCD_ErrLog((char*) "PWM thread terminated\n");
        }
    	pwm_ready = 0;
        osThreadTerminate(NULL);
    }
}
