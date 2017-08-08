#ifndef __SERVO_CONTROL_H_
#define __SERVO_CONTROL_H_

#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lcd_log.h"
#include "robot_arm_conf.h"
#include <string.h>

#define SERVOS				4
#define MIN_ADC_VALUE		0
#define MAX_ADC_VALUE		4095

/* SERVO 0 Configuration */

// PWM D3 PB4 TIM3_CH1
// ADC A0 PA0 ADC3_IN0
#define SERVO0_INST			TIM3
#define SERVO0_PERIOD		0xffff
#define SERVO0_PRESCALER	30
#define SERVO0_MIN_PULSE	2250
#define SERVO0_MAX_PULSE	7500
#define SERVO0_MIN_ANGLE	0
#define SERVO0_MAX_ANGLE	174
#define SERVO0_PWM_PORT		GPIOB
#define SERVO0_PWM_PIN		GPIO_PIN_4
#define SERVO0_PWM_AF		GPIO_AF2_TIM3
#define SERVO0_ADC_PORT		GPIOA
#define SERVO0_ADC_PIN		GPIO_PIN_0
#define SERVO0_ADC_CHANNEL	ADC_CHANNEL_0

/* SERVO 1 Configuration */

// PWM D6 PH6 TIM12_CH1
// ADC A1 PF10 ADC3_IN8
#define SERVO1_INST			TIM12
#define SERVO1_PERIOD		0xffff
#define SERVO1_PRESCALER	30
#define SERVO1_MIN_PULSE	2150
#define SERVO1_MAX_PULSE	7800
#define SERVO1_MIN_ANGLE	0
#define SERVO1_MAX_ANGLE	150
#define SERVO1_PWM_PORT		GPIOH
#define SERVO1_PWM_PIN		GPIO_PIN_6
#define SERVO1_PWM_AF		GPIO_AF9_TIM12
#define SERVO1_ADC_PORT		GPIOF
#define SERVO1_ADC_PIN		GPIO_PIN_10
#define SERVO1_ADC_CHANNEL	ADC_CHANNEL_8

/* SERVO 2 Configuration */

// PWM D9 PA15 TIM2_CH1
// ADC A2 PF9 ADC3_IN7
#define SERVO2_INST			TIM2
#define SERVO2_PERIOD		0xffff
#define SERVO2_PRESCALER	30
#define SERVO2_MIN_PULSE	2250
#define SERVO2_MAX_PULSE	7600
#define SERVO2_MIN_ANGLE	0
#define SERVO2_MAX_ANGLE	160
#define SERVO2_PWM_PORT		GPIOA
#define SERVO2_PWM_PIN		GPIO_PIN_15
#define SERVO2_PWM_AF		GPIO_AF1_TIM2
#define SERVO2_ADC_PORT		GPIOF
#define SERVO2_ADC_PIN		GPIO_PIN_9
#define SERVO2_ADC_CHANNEL	ADC_CHANNEL_7

/* SERVO 3 Configuration */

// PWM D10 PA8 TIM1_CH1
// ADC A3 PF8 ADC3_IN6
#define SERVO3_INST			TIM1
#define SERVO3_PERIOD		0xffff
#define SERVO3_PRESCALER	60
#define SERVO3_MIN_PULSE	1900
#define SERVO3_MAX_PULSE	8500
#define SERVO3_MIN_ANGLE	0
#define SERVO3_MAX_ANGLE	174
#define SERVO3_PWM_PORT		GPIOA
#define SERVO3_PWM_PIN		GPIO_PIN_8
#define SERVO3_PWM_AF		GPIO_AF1_TIM1
#define SERVO3_ADC_PORT		GPIOF
#define SERVO3_ADC_PIN		GPIO_PIN_8
#define SERVO3_ADC_CHANNEL	ADC_CHANNEL_6

typedef struct {
	TIM_TypeDef* instance;
	uint32_t period;
	uint32_t prescaler;
	uint32_t pulse;
} pwm_conf_t;

typedef struct {
	uint8_t min_angle;
	uint8_t max_angle;
	uint32_t min_pulse;
	uint32_t max_pulse;
	float adc_to_angle_const;
	float angle_to_pulse;
	float adc_to_pulse;
} servo_pos_conf_t;

typedef struct {
	uint32_t x;
	uint32_t y;
	uint32_t z;
} arm_pos_t;

typedef struct {
	uint32_t pulse;
	uint8_t angle;
} servo_pos_t;

uint16_t adc_values[SERVOS];
uint16_t adc_pulse_values[SERVOS];

uint32_t adc_channels[SERVOS];
ADC_HandleTypeDef adc;
ADC_ChannelConfTypeDef adc_ch[SERVOS];

pwm_conf_t pwm_conf[SERVOS];
TIM_HandleTypeDef pwm[SERVOS];
TIM_OC_InitTypeDef pwm_oc_init[SERVOS];

servo_pos_conf_t servo_pos_conf[SERVOS];
servo_pos_t servo_pos[SERVOS];
arm_pos_t arm_position;
uint8_t debug;
uint8_t adc_on;
uint8_t pwm_ready;
char lcd_log[100];

osMutexId servo_pos_mutex;

void servo_config(void);
void pwm_init(void);
void pwm_set_pulse(uint8_t servo, uint32_t pulse);
void adc_init(void);
void adc_deinit(void);
void adc_measure(void);
uint8_t adc_to_angle(uint8_t servo, uint16_t adc_value);
uint32_t angle_to_pulse(uint8_t servo, uint8_t degree);
uint32_t adc_to_pulse(uint8_t servo, uint16_t adc_value);
void pwm_thread(void const * argument);
void ttr_start_adc(void);
void ttr_stop_adc(void);
void adc_thread(void const * argument);
uint16_t map(uint16_t value, uint16_t min1, uint16_t max1, uint16_t min2, uint16_t max2);

#endif /* __SERVO_CONTROL_H_ */
