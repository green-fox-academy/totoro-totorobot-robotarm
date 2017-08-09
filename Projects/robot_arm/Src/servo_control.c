#include "servo_control.h"

void servo_config(void)
{
	// Configure up servo related data

	pwm_conf[0].instance = SERVO0_INST;
	pwm_conf[0].period = SERVO0_PERIOD;
	pwm_conf[0].prescaler = SERVO0_PRESCALER;
	pwm_conf[0].pulse = (SERVO0_MIN_PULSE + SERVO0_MAX_PULSE) / 2;

	pwm_conf[1].instance = SERVO1_INST;
	pwm_conf[1].period = SERVO1_PERIOD;
	pwm_conf[1].prescaler = SERVO1_PRESCALER;
	pwm_conf[1].pulse = (SERVO1_MIN_PULSE + SERVO1_MAX_PULSE) / 2;

	pwm_conf[2].instance = SERVO2_INST;
	pwm_conf[2].period = SERVO2_PERIOD;
	pwm_conf[2].prescaler = SERVO2_PRESCALER;
	pwm_conf[2].pulse = (SERVO2_MIN_PULSE + SERVO2_MAX_PULSE) / 2;

	pwm_conf[3].instance = SERVO3_INST;
	pwm_conf[3].period = SERVO3_PERIOD;
	pwm_conf[3].prescaler = SERVO3_PRESCALER;
	pwm_conf[3].pulse = (SERVO3_MIN_PULSE + SERVO3_MAX_PULSE) / 2;

	adc_ch_conf[0] = SERVO0_ADC_CHANNEL;
	adc_ch_conf[1] = SERVO1_ADC_CHANNEL;
	adc_ch_conf[2] = SERVO2_ADC_CHANNEL;
	adc_ch_conf[3] = SERVO3_ADC_CHANNEL;

	servo_pos_conf[0].min_angle = SERVO0_MIN_ANGLE;
	servo_pos_conf[0].max_angle = SERVO0_MAX_ANGLE;
	servo_pos_conf[0].min_pulse = SERVO0_MIN_PULSE;
	servo_pos_conf[0].max_pulse = SERVO0_MAX_PULSE;

	servo_pos_conf[1].min_angle = SERVO1_MIN_ANGLE;
	servo_pos_conf[1].max_angle = SERVO1_MAX_ANGLE;
	servo_pos_conf[1].min_pulse = SERVO1_MIN_PULSE;
	servo_pos_conf[1].max_pulse = SERVO1_MAX_PULSE;

	servo_pos_conf[2].min_angle = SERVO2_MIN_ANGLE;
	servo_pos_conf[2].max_angle = SERVO2_MAX_ANGLE;
	servo_pos_conf[2].min_pulse = SERVO2_MIN_PULSE;
	servo_pos_conf[2].max_pulse = SERVO2_MAX_PULSE;


	servo_pos_conf[3].min_angle = SERVO3_MIN_ANGLE;
	servo_pos_conf[3].max_angle = SERVO3_MAX_ANGLE;
	servo_pos_conf[3].min_pulse = SERVO3_MIN_PULSE;
	servo_pos_conf[3].max_pulse = SERVO3_MAX_PULSE;


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
		// sprintf(lcd_log, "Servo%d pulse: %5d\n", servo, pulse);
		// LCD_UsrLog(lcd_log);
	}

	return;
}

void adc_init(void)
{
	// General ADC init
	adc.State = HAL_ADC_STATE_RESET;
	adc.Instance = ADC3;
	adc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	adc.Init.Resolution = ADC_RESOLUTION_12B;
	adc.Init.EOCSelection = DISABLE;
	adc.Init.DMAContinuousRequests = DISABLE;
	adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc.Init.ContinuousConvMode = DISABLE;
	adc.Init.DiscontinuousConvMode = DISABLE;
	adc.Init.ScanConvMode = DISABLE;
	adc.Init.NbrOfConversion = 4;
	HAL_ADC_Init(&adc);

	// ADC channel general init
	// Channel selection will be set in adc_measure
	adc_ch.Channel = adc_ch_conf[0];
	adc_ch.Offset = 0;
	adc_ch.Rank = 1;
	adc_ch.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);

	return;
}

void adc_deinit(void)
{
	HAL_ADC_Stop(&adc);
	return;
}

void adc_measure(void)
{


	for (int i = 0; i < SERVOS; i++) {

		// Select channel
		adc_ch.Channel = adc_ch_conf[i];
		HAL_ADC_ConfigChannel(&adc, &adc_ch);

		// Measure
		HAL_ADC_Start(&adc);
		HAL_ADC_PollForConversion(&adc, HAL_MAX_DELAY);
		adc_values[i] = HAL_ADC_GetValue(&adc);

		// Calculate PMW pulse width
		adc_pulse_values[i] = map(adc_values[i], MIN_ADC_VALUE, MAX_ADC_VALUE, servo_pos_conf[i].min_pulse, servo_pos_conf[i].max_pulse);

		osDelay(5);
	}

	return;
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

void start_adc_thread(void)
{
	adc_init();
	adc_on = 1;

	if (debug) {
		LCD_UsrLog((char*) "ADC thread started\n");
	}

    osThreadDef(ADC_MEASURE, adc_thread, osPriorityLow, 0, configMINIMAL_STACK_SIZE * 5);
    osThreadCreate (osThread(ADC_MEASURE), NULL);

	return;
}

void stop_adc_thread(void)
{
	adc_on = 0;
	osDelay(100);
	adc_deinit();

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

			// TODO : check if ADC is running. If so, convert ADC values to pulse here
			// if (adc_on) { .... }

			// Lock mutex
			osMutexWait(servo_pos_mutex, osWaitForever);

			// Get pulse value
			uint32_t servo_pulse = servo_pos[i].pulse;

			// Release mutex
			osMutexRelease(servo_pos_mutex);

			// Set position
			pwm_set_pulse(i, servo_pulse);


		}
		osDelay(10);
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

void adc_thread(void const * argument)
{

	if (debug) {
		LCD_UsrLog((char*) "ADC thread started\n");
	}

	while(adc_on) {
		adc_measure();
		if (debug) {
			sprintf(lcd_log, "ADC0: %4d  ADC1: %4d  ADC2: %4d  ADC3: %4d\n", adc_values[0], adc_values[1], adc_values[2], adc_values[3]);
			LCD_UsrLog(lcd_log);
		}

		for (int i = 0; i < SERVOS; i++) {



			// Lock mutex
			osMutexWait(servo_pulse_mutex, osWaitForever);

			// Get pulse value
			servo_pulse[i] = adc_pulse_values[i];

			// Release mutex
			osMutexRelease(servo_pulse_mutex);
		}

		osDelay(10);
	}


	while (1) {
		// Terminate thread
		if (debug) {
			LCD_UsrLog((char*) "ADC thread terminated\n");
		}
		osThreadTerminate(NULL);
	}
}

double map(double input, double min_in, double max_in, double min_out, double max_out)
{
	double ratio =  input / (max_in - min_in);
	double output = (max_out - min_out) * ratio + min_out;

	return output;
}
